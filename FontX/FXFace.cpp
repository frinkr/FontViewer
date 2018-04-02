#include "FXFace.h"
#include "FXLib.h"
#include "FXPrivate.h"

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
//                     PRIVATE IMPL
//
bool
FXFace::init() {
    if (FT_New_Face(FXLib::get(), desc_.filePath.c_str(), desc_.faceIndex, &face_))
        return false;

    return initAttributes() && initCMap();
}

bool
FXFace::initAttributes() {
    atts_.index = desc_.faceIndex;

    const char * psName = FT_Get_Postscript_Name(face_);
    if (psName) atts_.postscriptName = psName;

    atts_.upem = face_->units_per_EM;       

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
