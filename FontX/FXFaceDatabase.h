#pragma once
#include "FXFace.h"

class FXFaceDatabase {
public:
    explicit FXFaceDatabase(const FXVector<FXString> & folders, const FXString & dbPath);

    size_t
    faceCount() const;

    const FXFaceDescriptor &
    faceDescriptor(size_t index);

    const FXFaceAttributes &
    faceAttributes(size_t index) const;
    
    FXPtr<FXFace>
    createFace(const FXFaceDescriptor & descriptor) const;

    void
    rescan();

protected:
    FXVector<FXString> folders_;
    FXString           dbPath_;
    FXVector<FXFaceDescriptor>  faces_;
    FXVector<FXFaceAttributes>  attrs_;
};
