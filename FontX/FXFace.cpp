#include "FXFace.h"
#include "FXLib.h"
#include "FXPrivate.h"

FXPtr<FXFace>
FXFace::createFace(const FVFaceDescriptor & descriptor) {
    return FXPtr<FXFace>(new FXFace(descriptor));
}
    
FXPtr<FXFace>
FXFace::createFace(const std::string & filePath, size_t faceIndex) {
    FVFaceDescriptor descritptor = {filePath, faceIndex};
    return FXFace::createFace(descritptor);
}

FXFace::FXFace(const FVFaceDescriptor & descriptor)
: descriptor_(descriptor)
, face_(nullptr) {
    init();
}

TXFTFace
FXFace::face() const {
    return face_;
}

size_t
FXFace::index() const {
    return descriptor_.faceIndex;
}

std::string
FXFace::postscriptName() const {
    const char * psName = FT_Get_Postscript_Name(face_);
    if (psName) return psName;
    return std::string();
}


bool
FXFace::init() {
    if (FT_New_Face(FXLib::get(), descriptor_.filePath.c_str(), descriptor_.faceIndex, &face_))
        return false;
    return true;
}
