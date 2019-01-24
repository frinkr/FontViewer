#pragma once
#include "FXFace.h"

class FXFaceDatabase {
public:
    explicit FXFaceDatabase(const FXVector<FXString> & folders, const FXString & dbPath);

    size_t
    faceCount() const;

    size_t
    faceIndex(const FXFaceDescriptor & desc) const;

    const FXFaceDescriptor &
    faceDescriptor(size_t index) const;

    const FXFaceAttributes &
    faceAttributes(size_t index) const;
    
    FXPtr<FXFace>
    createFace(const FXFaceDescriptor & descriptor) const;

    void
    rescan();

public:
    struct FaceItem {
        FXFaceDescriptor desc;
        FXFaceAttributes atts;   
    };

protected:
    bool
    save();
    
    bool
    load();

    bool
    checkUpdate() const;
    
protected:
    FXVector<FXString> folders_;
    FXString           dbPath_;

    FXVector<FaceItem> faces_;
    size_t             hash_;

    //FXVector<FXFaceDescriptor>  faces_;
    //FXVector<FXFaceAttributes>  attrs_;
};
