#include "FXFace.h"
#include "FXLib.h"
#include "FXPrivate.h"

double
pt2px(double p, double dpi) {
    return (p * dpi + 36) / 72.0;
}

FXPtr<FXFace>
FXFace::createFace(const FXFaceDescriptor & descriptor) {
    return FXPtr<FXFace>(new FXFace(descriptor));
}
    
FXPtr<FXFace>
FXFace::createFace(const std::string & filePath, size_t faceIndex) {
    FXFaceDescriptor descritptor = {filePath, faceIndex};
    return FXFace::createFace(descritptor);
}

FXFace::FXFace(const FXFaceDescriptor & descriptor)
: desc_(descriptor)
, face_(nullptr) {
    init();
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
    return atts_.index;
}

std::string
FXFace::postscriptName() const {
    return atts_.postscriptName;
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

FXCMap
FXFace::currentCMap() const {
    return cmaps()[currentCMapIndex()];
}

size_t
FXFace::currentCMapIndex() const {
    const FT_CharMap & curr = face_->charmap;
    for (FT_Int i = 0; i < face_->num_charmaps; ++ i) 
        if (face_->charmaps[i] == curr)
            return i;
    return -1;
}

//////////////////////////////////////////////////////////////////////////////////////////
//                     GLYPH
//

FXGlyph
FXFace::glyph(FXChar c, bool isGID) {
    FXGlyphID id = c;
    if (!isGID)
        FT_Get_Char_Index(face_, c);
    
    FXGlyph glyph;
    glyph.id = id;
    FT_Load_Glyph(face_, id, FT_LOAD_NO_SCALE);
    FT_GlyphSlot slot = face_->glyph;

    glyph.metrics.width        = slot->metrics.width;
    glyph.metrics.height       = slot->metrics.height;
    glyph.metrics.horiBearingX = slot->metrics.horiBearingX;
    glyph.metrics.horiBearingY = slot->metrics.horiBearingY;
    glyph.metrics.horiAdvance  = slot->metrics.horiAdvance;
    glyph.metrics.vertBearingX = slot->metrics.vertBearingX;
    glyph.metrics.vertBearingY = slot->metrics.vertBearingY;
    glyph.metrics.vertAdvance  = slot->metrics.vertAdvance;

    char glyphName[256] = {0};
    if (!FT_Get_Glyph_Name(face_, id, glyphName, sizeof(glyphName))) 
        glyph.name = glyphName;

    // let's render pixmap
    FT_Load_Glyph(face_, id, FT_LOAD_RENDER);
    slot = face_->glyph;
    FT_Bitmap ftBm = slot->bitmap;
    glyph.bitmap = FXBitmapARGB(ftBm.width, ftBm.rows);
    for (size_t y = 0; y < ftBm.rows; ++ y) {
        for (size_t x = 0; x < ftBm.width; ++ x) {
            uint8_t a = ftBm.buffer[y * ftBm.pitch + x];
            glyph.bitmap.setPixel(x, y, makeARGB(a, FXBlack)); 
        }
    }
    return glyph;
}
    
//////////////////////////////////////////////////////////////////////////////////////////
//                     PRIVATE IMPL
//
bool
FXFace::init() {
    if (FT_New_Face(FXLib::get(), desc_.filePath.c_str(), desc_.faceIndex, &face_))
        return false;

    FT_Set_Char_Size(face_, FXDefaultFontSize * 64, FXDefaultFontSize * 64, FXDefaultDPI, FXDefaultDPI);
    return initAttributes() && initCMap();
}

bool
FXFace::initAttributes() {
    atts_.index = desc_.faceIndex;

    const char * psName = FT_Get_Postscript_Name(face_);
    if (psName) atts_.postscriptName = psName;

    atts_.upem = face_->units_per_EM;       
    atts_.glyphCount = face_->num_glyphs;
    return true;
}

bool
FXFace::initCMap() {
    for (FT_Int i = 0; i < face_->num_charmaps; ++ i)  {
        const FT_CharMap & cm = face_->charmaps[i];
        FXCMap fxcm(cm->platform_id, cm->encoding_id);
        cmaps_.push_back(fxcm);
    }
    
    return true;
}
