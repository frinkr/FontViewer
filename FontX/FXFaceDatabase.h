#pragma once
#include "FXFace.h"

class FXFaceDatabase {
public:
    explicit FXFaceDatabase(const FXVector<FXString> & folders, const FXString & dbPath);
    
    const FXVector<FXFaceDescriptor> &
    faces() const;

    const FXFaceAttributes &
    faceAttributes(const FXFaceDescriptor & descriptor) const;
    
    FXPtr<FXFace>
    createFace(const FXFaceDescriptor & descriptor) const;

    void
    rescan();

protected:
    FXVector<FXString> folders_;
    FXString           dbPath_;
    FXVector<FXFaceDescriptor>  faces_;
    FXMap<FXFaceDescriptor, FXFaceAttributes> map_;
};
