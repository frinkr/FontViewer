#pragma once
#include "FXFace.h"

class FXFaceDatabase {
public:
    explicit FXFaceDatabase(const FXVector<FXString> & folders, const FXString & dbPath);

    const FXVector<FXFaceDescriptor> &
    faces() const;

    const FXFaceAttributes &
    faceAttributes(const FXFaceDescriptor & descriptor);
    
    FXPtr<FXFace>
    createFace(const FXFaceDescriptor & descriptor);

    void
    rescan();

protected:
    FXVector<FXString> folders_;
    FXString           dbPath_;
};
