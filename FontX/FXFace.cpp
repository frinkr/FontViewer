#include <cmath>

#include "FXFS.h"
#include "FXFTNames.h"
#include "FXFTPrivate.h"
#include "FXFace.h"
#include "FXCache.h"
#include "FXInspector.h"
#include "FXLib.h"
#include "FXPDF.h"
#include "PDF/FXPDFFace.h"

namespace {
    template <typename T>
    void forEachPixel(FT_Bitmap ftBm, size_t pixelSize, T func) {
        for (size_t y = 0; y < ftBm.rows; ++ y) {
            for (size_t x = 0; x < ftBm.width; ++ x) {
                unsigned char * p = ftBm.buffer + (y * ftBm.pitch + x * pixelSize);
                func(int(x), int(y), p);
            }
        }
    }
    
    FXGlyphImage
    loadPixmap(FT_Bitmap ftBm) {
        FXPixmapARGB bm(ftBm.width, ftBm.rows);
        FXGlyphImage::Mode mode {FXGlyphImage::kGrayscale};
        
        if (ftBm.pixel_mode == FT_PIXEL_MODE_GRAY) {
            forEachPixel(ftBm, 1, [&bm](int x, int y, unsigned char * p) {
                bm.setPixel(x, y, makeARGB(*p, FXBlack)); 
            });
        }
        else if (ftBm.pixel_mode == FT_PIXEL_MODE_BGRA) {
            mode = FXGlyphImage::kColor;
            forEachPixel(ftBm, 4, [&bm](int x, int y, unsigned char * p) {
                bm.setPixel(x, y, makeARGB(*(p+3), *(p+2), *(p+1), *p));
            });
        }
        else if (ftBm.pixel_mode == FT_PIXEL_MODE_MONO) {
            mode = FXGlyphImage::kMono;
            forEachPixel(ftBm, 0, [&bm](int x, int y, unsigned char * row) {
                unsigned char * b = row + x / 8;
                bool v = (*b) & (1 << (7 - (x % 8)));
                bm.setPixel(x, y, v? FXBlack: FXTransparent);
            });
        }
        
        return FXGlyphImage {bm, mode, {}};
    }
}

double
pt2px(double p, double dpi) {
    return (p * dpi + 36) / 72.0;
}

double
px2pt(double p, double dpi) {
    return (p * 72 + 36) / dpi;
}

bool
FXFaceDescriptor::operator==(const FXFaceDescriptor & other) const {
    return filePath == other.filePath && index == other.index;
}

bool
FXFaceDescriptor::operator<(const FXFaceDescriptor & other) const {
    if (filePath < other.filePath)
        return true;
    else if (filePath == other.filePath)
        return index < other.index;
    else
        return false;
}

bool
FXFaceDescriptor::operator!=(const FXFaceDescriptor & other) const {
    return !operator==(other);
}

static FXVector<FXFaceLanguage> sSFNTLanguageSearchOrder{
    FXFaceLanguages::zhCN, 
    FXFaceLanguages::zhHK, 
    FXFaceLanguages::zhTW, 
    FXFaceLanguages::zhSG, 
    FXFaceLanguages::ja, 
    FXFaceLanguages::ko, 
    FXFaceLanguages::en
};

const FXString &
FXFaceNames::familyName() const {
    if (familyName_.empty())
        familyName_ = findSFNTName({TT_NAME_ID_TYPOGRAPHIC_FAMILY,TT_NAME_ID_FONT_FAMILY, TT_NAME_ID_WWS_FAMILY},
                                   sSFNTLanguageSearchOrder,
                                   defaultFamilyName_);
    return familyName_;
}

const FXString &
FXFaceNames::styleName() const {
    if (styleName_.empty())
        styleName_ = findSFNTName({TT_NAME_ID_TYPOGRAPHIC_SUBFAMILY,TT_NAME_ID_FONT_SUBFAMILY, TT_NAME_ID_WWS_SUBFAMILY},
                                  sSFNTLanguageSearchOrder,
                                  defaultStyleName_);
    return styleName_;
}

const FXString &
FXFaceNames::postscriptName() const {
    if (postscriptName_.empty())
        postscriptName_ = findSFNTName({TT_NAME_ID_PS_NAME, TT_NAME_ID_CID_FINDFONT_NAME},
                                       sSFNTLanguageSearchOrder,
                                       defaultPostscriptName_);
    return postscriptName_;
}

FXHashMap<FXFaceLanguage, FXString>
FXFaceNames::localizedFamilyNames() const {
    return findSFNTNames({TT_NAME_ID_TYPOGRAPHIC_FAMILY,TT_NAME_ID_FONT_FAMILY, TT_NAME_ID_WWS_FAMILY});
}

FXHashMap<FXFaceLanguage, FXString>
FXFaceNames::localizedStyleNames() const {
    return findSFNTNames({TT_NAME_ID_TYPOGRAPHIC_SUBFAMILY,TT_NAME_ID_FONT_SUBFAMILY, TT_NAME_ID_WWS_SUBFAMILY});
}

FXHashMap<FXFaceLanguage, FXString>
FXFaceNames::localizedPostscriptNames() const {
    return findSFNTNames({TT_NAME_ID_PS_NAME, TT_NAME_ID_CID_FINDFONT_NAME});
}

void
FXFaceNames::setDefaultFamilyName(const FXString & name) {
    defaultFamilyName_ = name;
}

void
FXFaceNames::setDefaultStyleName(const FXString & name) {
    defaultStyleName_ = name;
}

void
FXFaceNames::setDefaultPostscriptName(const FXString & name) {
    defaultPostscriptName_ = name;
}

FXHashMap<FXFaceLanguage, FXString>
FXFaceNames::findSFNTNames(const FXVector<int> & nameIds) const {
    FXHashMap<FXFaceLanguage, FXString> map;
    for (auto & sfnt: sfntNames_) {
        if (sfnt.value.empty())
            continue;
        for (int nameId : nameIds) {
            if (sfnt.nameId == nameId) {
                map[sfnt.language] = sfnt.value;
            }
        }
    }
    return map;
}

FXString
FXFaceNames::getSFNTName(int nameId) const {
    auto names = findSFNTNames({nameId});
    auto itr = names.find("en");
    if (itr != names.end())
        return itr->second;
    if (names.size())
        return names.begin()->second;
    return FXString();
}

FXString
FXFaceNames::vendor() const {
    return getSFNTName(TT_NAME_ID_MANUFACTURER);
}

FXString
FXFaceNames::version() const {
    return getSFNTName(TT_NAME_ID_VERSION_STRING);
}


FXString
FXFaceNames::findSFNTName(const FXVector<int> & nameIds,
                          const FXVector<FXFaceLanguage> & languages,
                          const FXString & defaultName) const {
    for (auto & sfnt : sfntNames_) {
        if (sfnt.value.empty())
            continue;
        for (int nameId : nameIds) {
            if (sfnt.nameId == nameId) {
                for (const FXFaceLanguage & lang: languages) {
                    if (sfnt.language == lang)
                        return sfnt.value;
                }
                return sfnt.value;
            }
        }
    }
    return defaultName;
}

const FXString FXFaceFormatConstant::TrueType{"TrueType"};
const FXString FXFaceFormatConstant::Type1{"Type 1"};
const FXString FXFaceFormatConstant::CFF{"CFF"};
const FXString FXFaceFormatConstant::WinFNT{"Windows FNT"};
const FXString FXFaceFormatConstant::Other{"Other"};

FXPtr<FXFace>
FXFace::createFace(const FXFaceDescriptor & descriptor) {
	auto face = new FXFace(descriptor);
	if (face->hasValidFaceData())
        return FXPtr<FXFace>(face);
    else
        delete face;
#if FX_HAS_PDF_ADDON
    FXPtr<FXPDFDocument> pdf = FXPDFDocument::open(descriptor.filePath);
    if (pdf)
        return pdf->createFace(descriptor.index);
#endif
	return nullptr;
}
    
FXPtr<FXFace>
FXFace::createFace(const std::string & filePath, size_t faceIndex) {
    FXFaceDescriptor descriptor = {filePath, faceIndex};
    return FXFace::createFace(descriptor);
}

FXPtr<FXFace>
FXFace::createFace(FXPtr<FXStream> stream, size_t faceIndex) {
    auto face = FXPtr<FXFace>(new FXFace(stream, faceIndex));
    if (face->hasValidFaceData())
        return face;
    return nullptr;
}

FXPtr<FXFace>
FXFace::createFace(FXFTFace face) {
    return FXPtr<FXFace>(new FXFace(face));
}

///////////////////////////////////////////////////
FXFace::FXFace() {    
}

FXFace::FXFace(const FXFaceDescriptor & descriptor)
: desc_(descriptor)
, face_(nullptr) {
    if (FXFTOpenFace(FXLib::get(), desc_.filePath, desc_.index, &face_))
        return;
    init();
}

FXFace::FXFace(const std::string & filePath, size_t index)
    : FXFace(FXFaceDescriptor{filePath, index}){
}

FXFace::FXFace(FXFTFace face)
    : face_(face) {
    FT_Reference_Face(face_);
    init();
}

FXFace::FXFace(FXPtr<FXStream> stream, size_t faceIndex)
    : stream_(stream)
    , face_(nullptr) {
    if (FXFTOpenFace(FXLib::get(), stream_, faceIndex, &face_))
        return;
    init();
}

FXFace::~FXFace() {
    if (face_)
        FT_Done_Face(face_);
}

//////////////////////////////////////////////////////////////////////////////////////////
//                BASIC GETTERS
//
const FXFaceDescriptor &
FXFace::desc() const {
    return desc_;
}

bool
FXFace::hasValidFaceData() const {
    return face_ != nullptr;
}

bool
FXFace::isSubset() const {
    return false;
}


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

size_t
FXFace::faceCount() const {
    return face_->num_faces;
}

FXPtr<FXFace>
FXFace::openFace(size_t index) {
    if (index == this->index())
        return this->shared_from_this();
    
    FXFace * face = nullptr;
    if (stream_)
        face = new FXFace(stream_, index);
    else
        face = new FXFace(desc_.filePath, index);
    if (face->hasValidFaceData())
        return FXPtr<FXFace>(face);
    delete face;
    return nullptr;
}

const FXDict &
FXFace::userProperties() const {
    return properties_;
}

FXDict &
FXFace::userProperties() {
    return properties_;
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
    if (!face_->charmap)
        return 0; // dummy
    return static_cast<size_t>(FT_Get_Charmap_Index(face_->charmap));
}

bool
FXFace::selectCMap(size_t cmapIndex) {
    if (cmapIndex >= static_cast<size_t>(face_->num_charmaps))
        return false;
    return !FT_Set_Charmap(face_, face_->charmaps[cmapIndex]);
}

//////////////////////////////////////////////////////////////////////////////////////////
//                     SIZE
//

FXFace::AutoFontSize::AutoFontSize(FXFace * face, double fontSize)
    : face_(face) {
    oldSize_ = face_->face()->size;
    FT_New_Size(face_->face(), &newSize_);
    FT_Activate_Size(newSize_);
    
    face_->selectFontSize(fontSize);
}
    
FXFace::AutoFontSize::~AutoFontSize() {
    FT_Activate_Size(oldSize_);
    FT_Done_Size(newSize_);
}


double
FXFace::fontSize() const {
    return fontSize_;
}

bool
FXFace::isScalable() const {
    return scalable_;
}

double
FXFace::bmScale() const {
    return bmScale_;
}
    
double
FXFace::selectFontSize(double fontSize) {
    fontSize_ = fontSize;
    bmScale_ = 1;
    bmStrikeIndex_ = 0;
    
    if (scalable_) {
        FT_Set_Char_Size(face_, 0/*same as height*/, fontSize * 64, FXDefaultDPI, FXDefaultDPI);
    }
    else {
        if (face_->num_fixed_sizes) {
            // select the strike which is closest to required size
            const double pixSize = pt2px(fontSize_);
            int strikeIndex = 0;
            double diff = fabs(pixSize - face_->available_sizes[0].height);
            for (int i = 1; i < face_->num_fixed_sizes; ++i) {
                double ndiff = fabs(pixSize - face_->available_sizes[i].height);
                if (ndiff < diff) {
                    strikeIndex = i;
                    diff = ndiff;
                }
            }
            FT_Select_Size(face_, strikeIndex);
            
            bmScale_ = pixSize * 64 / face_->available_sizes[strikeIndex].x_ppem;;
            bmStrikeIndex_ = strikeIndex;
        }
        else {
            FT_Set_Char_Size(face_, 0/*same as height*/, fontSize * 64, FXDefaultDPI, FXDefaultDPI);
        }
    }

    return fontSize_;
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

    // lookup in cache
    if (glyphCache_->has(gid)) {
        FXGlyph glyph = glyphCache_->get(gid);
        glyph.character = {c, currentCMap().isUnicode()? FXGCharTypeUnicode : FXGCharTypeOther};
        return glyph;
    }
    
    FXGlyph glyph;
    glyph.face = this;
    glyph.gid = gid;
    glyph.character = {c, currentCMap().isUnicode()? FXGCharTypeUnicode : FXGCharTypeOther};
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

    // put into cache
    glyphCache_->put(gid, glyph);
    return glyph;
}

FXGlyphImage
FXFace::glyphImage(FXGlyphID gid) {
    if (fontSize_ == FXDefaultFontSize && glyphImageCache_->has(gid))
        return glyphImageCache_->get(gid);
    
    FT_Load_Glyph(face_, gid, FT_LOAD_RENDER | FT_LOAD_COLOR);

    auto image = loadPixmap(face_->glyph->bitmap);
    image.offset.x = face_->glyph->bitmap_left;
    image.offset.y = face_->glyph->bitmap_top - face_->glyph->bitmap.rows;
    image.emSize   = FXVec2d<int>{ int(pt2px(fontSize_)), int(pt2px(fontSize_)) };
    image.scale    = bmScale_;
    
    if (fontSize_ == FXDefaultFontSize)
        glyphImageCache_->put(gid, image);
    return image;
}

FXVector<FXChar>
FXFace::charsForGlyph(FXGlyphID gid) const {
    return currentCMap().charsForGlyph(gid);
}

//////////////////////////////////////////////////////////////////////////////////////////
//                     VARIABLES
//


const FXVector<FXFace::VariableAxis> &
FXFace::variableAxises() const {
    return variableAxises_;    
}

const FXVector<FXFace::VariableNamedInstance> &
FXFace::variableNamedInstances() const {
    return variableNamedInstances_;
}

const FXVector<FXFixed>
FXFace::currentVariableCoordinates() const {
    FXVector<FXFixed> coords(variableAxises_.size());
    FT_Get_Var_Design_Coordinates(
        face_,
        coords.size(),
        reinterpret_cast<FT_Fixed *>(&coords[0])
        );
    return coords;
}

bool
FXFace::setCurrentVariableCoordinates(const FXVector<FXFixed> & coords) {
    if (coords == currentVariableCoordinates())
        return false;

    FT_Set_Var_Design_Coordinates(
        face_,
        variableAxises_.size(),
        const_cast<FT_Fixed *>(reinterpret_cast<const FT_Fixed *>(&coords[0]))
        );
    
    clearCache();
    return true;
}

void
FXFace::resetVariableCoordinates() {
    FT_Set_Var_Design_Coordinates(face_, 0, nullptr);
    clearCache();
}


FXPtr<FXInspector>
FXFace::inspector() {
    if (!inspector_)
        inspector_.reset(new FXInspector(this));
    return inspector_;
}

//////////////////////////////////////////////////////////////////////////////////////////
//                     PRIVATE IMPL
//
bool
FXFace::init() {
    scalable_ = FT_IS_SCALABLE(face_);
    selectFontSize(FXDefaultFontSize);
    glyphCache_.reset(new FXGlyphCache(5000));
    glyphImageCache_.reset(new FXGlyphImageCache(1000));
    bool ok =
        initAttributes() &&
        initCMap() &&
        initVariables();
    
    // PostInit
    if (ok) {
        for (auto & cm: cmaps_) {
            if (cm.isUnicode()) {
                atts_.haveUnicodeCMap = true;
                break;
            }
        }
    }
    return ok;
}

bool
FXFace::initAttributes() {
    atts_.desc = desc_;
    atts_.upem = face_->units_per_EM;       
    atts_.glyphCount = face_->num_glyphs;
    atts_.bbox = FXMakeRect<fu>(face_->bbox.xMin, face_->bbox.yMax, face_->bbox.xMax, face_->bbox.yMin);
    atts_.ascender = face_->ascender;
    atts_.descender = face_->descender;
    const char * format = FT_Get_Font_Format(face_);
    if (format)
        atts_.format = format;
    
    // names
    if (face_->family_name)
        atts_.names.setDefaultFamilyName(face_->family_name);
    if (face_->style_name)
        atts_.names.setDefaultStyleName(face_->style_name);

    const char * psName = FT_Get_Postscript_Name(face_);
    if (psName)
        atts_.names.setDefaultPostscriptName(psName);
    
    if (!atts_.upem && psName) {
        if (!strcmp(psName, "AppleColorEmoji")) atts_.upem = 800;
        else atts_.upem = 1000;
    }
    
    FT_UInt sfntCount = FT_Get_Sfnt_Name_Count(face_);
    for (FT_UInt i = 0; i < sfntCount; ++i) {
        FT_SfntName sfnt;
        if (FT_Get_Sfnt_Name(face_, i, &sfnt))
            continue;

        FXSFNTName entry;
        entry.platformId = sfnt.platform_id;
        entry.encodingId = sfnt.encoding_id;
        entry.language   = FXSFNTGetLanguage(&sfnt, face_);
        entry.nameId     = sfnt.name_id;
        entry.value      = FXToString(sfnt.platform_id, sfnt.encoding_id, sfnt.string, sfnt.string_len);

        atts_.names.sfntNames().push_back(entry);
    }

    // cid
    FT_Bool isCID = false;
    atts_.isCID = !!(FT_IS_CID_KEYED(face_) || (!FT_Get_CID_Is_Internally_CID_Keyed(face_, &isCID) && isCID));
    const char * registry = nullptr, *ordering = nullptr;
    FT_Int supplement = 0;
    if (!FT_Get_CID_Registry_Ordering_Supplement(face_, &registry, &ordering, &supplement)) 
        atts_.cid = std::string(registry) + "-" + ordering + "-" + std::to_string(supplement);
	
#ifdef FT_FACE_FLAG_VARIATION
    atts_.isOpenTypeVariable = (FT_FACE_FLAG_VARIATION & face_->face_flags);
#else
	atts_.isOpenTypeVariable = false;
#endif
    atts_.isMultipleMaster = (FT_FACE_FLAG_MULTIPLE_MASTERS & face_->face_flags);

    return true;
}

bool
FXFace::initCMap() {
    if (!face_->num_charmaps) {
        if (atts_.isCID) {
            FXCMap cm(this, atts_.cid, 0, true);
            cmaps_.push_back(cm);
        }
        else {
            FXCMap fxcm(this, 999, 999, 0, true);
            cmaps_.push_back(fxcm);
        }
        return true;
    }
    
    std::optional<size_t> firstUnicodeCMapIndex;
    
    size_t current = currentCMapIndex();
    for (FT_Int i = 0; i < face_->num_charmaps; ++ i)  {
        const bool isValid = selectCMap(i);
        
        const FT_CharMap & cm = face_->charmaps[i];
        FXCMap fxcm(this, cm->platform_id, cm->encoding_id, i, isValid);
        cmaps_.push_back(fxcm);
        
        if (!firstUnicodeCMapIndex && fxcm.isUnicode())
            firstUnicodeCMapIndex = i;
    }
    
    selectCMap(current);
    if (!currentCMap().isUnicode() && firstUnicodeCMapIndex)
        selectCMap(*firstUnicodeCMapIndex);
    
    return true;
}

bool
FXFace::initVariables() {
    FT_MM_Var * var = nullptr;
    FT_Multi_Master  mm;
    bool isMM = false;

    if (!FT_HAS_MULTIPLE_MASTERS(face_))
        return true;

    if (FT_Get_MM_Var(face_, &var))
        var = nullptr;
    if (FT_Get_Multi_Master(face_, &mm) == 0) 
        isMM =  true;

    if (!var)
        return true;

    // Load axises
    for (FT_UInt i = 0; i < var->num_axis; ++ i) {
        const FT_Var_Axis * axis = var->axis + i;
        VariableAxis a;
        a.name     = atts_.names.getSFNTName(axis->strid);
        a.index    = i;
        a.tag      = axis->tag;
        a.minValue = axis->minimum;
        a.maxValue = axis->maximum;
        a.defaultValue = axis->def;

        variableAxises_.push_back(a);
    }

    if (isMM)
        return true;

    // Load named instances
    for (FT_UInt i = 0; i < var->num_namedstyles; ++ i) {
        const FT_Var_Named_Style * style = var->namedstyle + i;
        VariableNamedInstance instance;
        instance.index      = i;
        instance.name       = atts_.names.getSFNTName(style->strid);
        instance.psName     = atts_.names.getSFNTName(style->psid);

        bool isDefault = true;
        for (FT_UInt j = 0; j < var->num_axis; ++ j) {
            instance.coordinates.push_back(style->coords[j]);
            if (style->coords[j] != variableAxises_[j].defaultValue)
                isDefault = false;
        }
        
        instance.isDefault = isDefault;
        variableNamedInstances_.push_back(instance);
    }
    
    size_t defaultInstance = -1;
    for (size_t i = 0; i < variableNamedInstances_.size(); ++ i) {
        if (variableNamedInstances_[i].isDefault) {
            defaultInstance = i;
        }
    }
    
    if (defaultInstance == -1) {
        // synthersize a default instance
        VariableNamedInstance instance;
        instance.index      = 0;
        instance.name       = "<default>";
        instance.psName     = postscriptName();
        instance.isDefault  = true;
        for (const auto & ax : variableAxises_)
            instance.coordinates.push_back(ax.defaultValue);
        variableNamedInstances_.insert(variableNamedInstances_.begin(), instance);
        
        for (size_t i = 1; i < variableNamedInstances_.size(); ++ i)
            variableNamedInstances_[i].index = i;
        
        defaultInstance = 0;
    }
    
    // Select the default -- this fixes the crashing on macOS for variable font
    setCurrentVariableCoordinates(variableNamedInstances_[defaultInstance].coordinates);
    
    return true;
}

void
FXFace::clearCache() {
    if (glyphCache_)
        glyphCache_->clear();
    if (glyphImageCache_)
        glyphImageCache_->clear();
}


/////////////////////////////////////////////////////////////////////////////
// FXFastFace

FXPtr<FXFastFace>
FXFastFace::create(const FXFaceDescriptor & descriptor) {
    FXFTFace face = nullptr;
    if (FXFTOpenFace(FXLib::get(), descriptor.filePath, descriptor.index, &face))
        return FXPtr<FXFastFace>();
    return FXPtr<FXFastFace>(new FXFastFace(face));
}

FXPtr<FXFastFace>
FXFastFace::create(FXPtr<FXStream> stream, size_t faceIndex) {
    FXFTFace face = nullptr;
    if (FXFTOpenFace(FXLib::get(), stream, faceIndex, &face))
        return FXPtr<FXFastFace>();
    return FXPtr<FXFastFace>(new FXFastFace(face));
}

FXFastFace::FXFastFace(FXFTFace face)
: face_(face) {
    assert(face);
    FT_Reference_Face(face_);
}

FXFastFace::~FXFastFace() {
    if (face_)
        FT_Done_Face(face_);
}

FXGlyphID
FXFastFace::glyphIDForChar(FXChar ch) const {
    return FT_Get_Char_Index(face_, ch);
}

bool
FXFastFace::hasGlyphForChar(FXChar ch) const {
    return glyphIDForChar(ch) != 0;
}

FXFTFace
FXFastFace::face() const {
    return face_;
}
