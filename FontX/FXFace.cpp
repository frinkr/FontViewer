#include "FXFace.h"
#include "FXLib.h"
#include "FXFTNames.h"
#include "FXFTPrivate.h"
#include "FXBoostPrivate.h"

namespace {
    template <typename T>
    void forEachPixel(FT_Bitmap ftBm, size_t pixelSize, T func) {
        for (size_t y = 0; y < ftBm.rows; ++ y) {
            for (size_t x = 0; x < ftBm.width; ++ x) {
                unsigned char * p = ftBm.buffer + (y * ftBm.pitch + x * pixelSize);
                func(x, y, p);
            }
        }
    }
    
    FXBitmapARGB
    loadBitmap(FT_Bitmap ftBm) {
        FXBitmapARGB bm(ftBm.width, ftBm.rows);

        
        if (ftBm.pixel_mode == FT_PIXEL_MODE_GRAY) {
            forEachPixel(ftBm, 1, [&bm](int x, int y, unsigned char * p) {
                bm.setPixel(x, y, makeARGB(*p, FXBlack)); 
            });
        }
        else if (ftBm.pixel_mode == FT_PIXEL_MODE_BGRA) {
            forEachPixel(ftBm, 4, [&bm](int x, int y, unsigned char * p) {
                bm.setPixel(x, y, makeARGB(*(p+3), *(p+2), *(p+1), *p));
            });
        }
        else if (ftBm.pixel_mode == FT_PIXEL_MODE_MONO) {
            forEachPixel(ftBm, 0, [&bm](int x, int y, unsigned char * row) {
                unsigned char * b = row + x / 8;
                bool v = (*b) & (1 << (7 - (x % 8)));
                bm.setPixel(x, y, v? FXBlack: FXWhite);
            });
        }
        return bm;
    }
}

double
pt2px(double p, double dpi) {
    return (p * dpi + 36) / 72.0;
}

FXString
FXFaceNames::familyName() const {
    return findSFNTName({TT_NAME_ID_TYPOGRAPHIC_FAMILY,TT_NAME_ID_FONT_FAMILY, TT_NAME_ID_WWS_FAMILY},
                        {"zh-cn", "zh-hk", "zh-tw", "zh-sg", "zh", "ja", "ko", "en"},
                        familyName_);
}

FXString
FXFaceNames::styleName() const {
    return findSFNTName({TT_NAME_ID_TYPOGRAPHIC_SUBFAMILY,TT_NAME_ID_FONT_SUBFAMILY, TT_NAME_ID_WWS_SUBFAMILY},
                        {"zh-cn", "zh-hk", "zh-tw", "zh-sg", "zh", "ja", "ko", "en"},
                        styleName_);
}

FXString
FXFaceNames::postscriptName() const {
    return findSFNTName({TT_NAME_ID_PS_NAME, TT_NAME_ID_CID_FINDFONT_NAME},
                        {"zh-cn", "zh-hk", "zh-tw", "zh-sg", "zh", "ja", "ko", "en"},
                        postscriptName_);
}

void
FXFaceNames::setFamilyName(const FXString & name) {
    familyName_ = name;
}

void
FXFaceNames::setStyleName(const FXString & name) {
    styleName_ = name;
}

void
FXFaceNames::setPostscriptName(const FXString & name) {
    postscriptName_ = name;
}

FXString
FXFaceNames::findSFNTName(const FXVector<int> & nameIds,
                          const FXVector<FXString> & languages,
                          const FXString & defaultName) const {
    auto findName = [&] () {
        for (auto itr = cbegin(); itr != cend(); ++ itr) {
            if (itr->value.empty())
                continue;
            for (int nameId : nameIds) {
                if (itr->nameId == nameId) {
                    for (const FXString & lang: languages) {
                        if (itr->language == lang)
                            return itr->value;
                    }
                    return itr->value;
                }
            }
        }
        return FXString();
    };
    FXString name = findName();
    if (name.empty())
        name = defaultName;
    return name;
}

FXPtr<FXFace>
FXFace::createFace(const FXFaceDescriptor & descriptor) {
    return FXPtr<FXFace>(new FXFace(descriptor));
}
    
FXPtr<FXFace>
FXFace::createFace(const std::string & filePath, size_t faceIndex) {
    FXFaceDescriptor descriptor = {filePath, faceIndex};
    return FXFace::createFace(descriptor);
}

FXPtr<FXFace>
FXFace::createFace(FXFTFace face) {
    return FXPtr<FXFace>(new FXFace(face));
}

FXFace::FXFace(const FXFaceDescriptor & descriptor)
: desc_(descriptor)
, face_(nullptr) {
    if (FT_New_Face(FXLib::get(), desc_.filePath.c_str(), FT_Long(desc_.index), &face_))
        return;
    init();
}

FXFace::FXFace(FXFTFace face)
    : face_(face) {
    FT_Reference_Face(face_);
    init();
}

FXFace::~FXFace() {
    if (face_)
        FT_Done_Face(face_);
}

//////////////////////////////////////////////////////////////////////////////////////////
//                BASIC GETTERS
//
FXFTFace
FXFace::face() const {
    return face_;
}

size_t
FXFace::index() const {
    return atts_.desc.index;
}

std::string
FXFace::postscriptName() const {
    return atts_.names.postscriptName();
}

size_t
FXFace::upem() const {
    return atts_.upem;
}

size_t
FXFace::glyphCount() const {
    return atts_.glyphCount;
}

const FXFaceAttributes &
FXFace::attributes() const {
    return atts_;
}





//////////////////////////////////////////////////////////////////////////////////////////
//                     CMAPS
//
const std::vector<FXCMap> &
FXFace::cmaps() const {
    return cmaps_;
}

const FXCMap &
FXFace::currentCMap() const {
    return cmaps_[currentCMapIndex()];
}

size_t
FXFace::currentCMapIndex() const {
	return FT_Get_Charmap_Index(face_->charmap);
}

bool
FXFace::selectCMap(size_t cmapIndex) {
    if (cmapIndex >= face_->num_charmaps)
        return false;
    return !FT_Set_Charmap(face_, face_->charmaps[cmapIndex]);
}

//////////////////////////////////////////////////////////////////////////////////////////
//                     GLYPH
//

FXGlyph
FXFace::glyph(FXGChar gc) {
    FXGlyphID gid = gc.value;
    FXChar c = gc.value;
    if (FXGCharTypeGlyphID == gc.type) {
        const auto chs = charsForGlyph(gid);
        if (chs.size())
            c = chs[0];
        else
            c = FXCharInvalid;
    }
    else {
        gid = FT_Get_Char_Index(face_, c);
    }
    FXGlyph glyph;
    glyph.gid = gid;
    glyph.character = {currentCMap().isUnicode()? FXGCharTypeUnicode : FXGCharTypeOther, c};
    FT_Load_Glyph(face_, gid, FT_LOAD_NO_SCALE);
    FT_GlyphSlot slot = face_->glyph;

    glyph.metrics.width        = (fu)slot->metrics.width;
    glyph.metrics.height       = (fu)slot->metrics.height;
    glyph.metrics.horiBearingX = (fu)slot->metrics.horiBearingX;
    glyph.metrics.horiBearingY = (fu)slot->metrics.horiBearingY;
    glyph.metrics.horiAdvance  = (fu)slot->metrics.horiAdvance;
    glyph.metrics.vertBearingX = (fu)slot->metrics.vertBearingX;
    glyph.metrics.vertBearingY = (fu)slot->metrics.vertBearingY;
    glyph.metrics.vertAdvance  = (fu)slot->metrics.vertAdvance;

    char glyphName[256] = {0};
    if (!FT_Get_Glyph_Name(face_, gid, glyphName, sizeof(glyphName))) 
        glyph.name = glyphName;

    // let's render pixmap
    FT_Load_Glyph(face_, gid, FT_LOAD_RENDER | FT_LOAD_COLOR);
    glyph.bitmap = loadBitmap(face_->glyph->bitmap);
    return glyph;
}

FXVector<FXChar>
FXFace::charsForGlyph(FXGlyphID gid) const {
    return currentCMap().charsForGlyph(gid);
}

//////////////////////////////////////////////////////////////////////////////////////////
//                     PRIVATE IMPL
//
bool
FXFace::init() {
    FT_Set_Char_Size(face_, 
        FT_UInt(FXDefaultFontSize * 64), 
        FT_UInt(FXDefaultFontSize * 64),
        FT_UInt(FXDefaultDPI),
        FT_UInt(FXDefaultDPI));
    return initAttributes() && initCMap();
}

bool
FXFace::initAttributes() {
    atts_.desc = desc_;
    atts_.upem = face_->units_per_EM;       
    atts_.glyphCount = face_->num_glyphs;
    atts_.fileName = BFS::fileName(desc_.filePath);

    const char * format = FT_Get_Font_Format(face_);
    if (format)
        atts_.format = format;
    
    if (face_->family_name)
        atts_.names.setFamilyName(face_->family_name);
    if (face_->style_name)
        atts_.names.setStyleName(face_->style_name);

    const char * psName = FT_Get_Postscript_Name(face_);
    if (psName)
        atts_.names.setPostscriptName(psName);
    
    FT_UInt sfntCount = FT_Get_Sfnt_Name_Count(face_);
    for (FT_UInt i = 0; i < sfntCount; ++i) {
        FT_SfntName sfnt;
        if (FT_Get_Sfnt_Name(face_, i, &sfnt))
            continue;

        FXSFNTName entry;
        entry.platformId = sfnt.platform_id;
        entry.encodingId = sfnt.encoding_id;
        entry.language   = FXGetLanguageName(sfnt.platform_id, sfnt.language_id);
        entry.nameId     = sfnt.name_id;
        entry.value      = FXToString(sfnt.platform_id, sfnt.encoding_id, sfnt.string, sfnt.string_len);

        atts_.names.push_back(entry);
    }
    return true;
}

bool
FXFace::initCMap() {
    size_t current = currentCMapIndex();
    for (FT_Int i = 0; i < face_->num_charmaps; ++ i)  {
        if (!selectCMap(i))
            continue;
        
        const FT_CharMap & cm = face_->charmaps[i];
        FXCMap fxcm(this, cm->platform_id, cm->encoding_id, i);
        cmaps_.push_back(fxcm);
    }
    
    selectCMap(current);
    
    return true;
}
