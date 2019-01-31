#pragma once
#include <functional>
#include "FXFace.h"



class FXFaceDatabase {
public:
    using ProgressCallback = std::function<bool(size_t current, size_t total, const FXString & file)>;
public:
    explicit FXFaceDatabase(const FXVector<FXString> & folders, const FXString & dbPath, ProgressCallback progressCallback = ProgressCallback{});

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
        size_t           fileHash;
        FXFaceDescriptor desc;
        FXFaceAttributes atts;   
    };

protected:
    bool
    save();
    
    bool
    load();

    bool
    checkUpdate();
    
protected:
    FXVector<FXString> folders_;
    FXSet<FXString>    files_;
    FXString           dbPath_;

    FXVector<FaceItem> faces_;
    size_t             hash_;

    ProgressCallback   progress_;

    FXMap<FXString, FXVector<size_t>> hashMap_;
    //FXVector<FXFaceDescriptor>  faces_;
    //FXVector<FXFaceAttributes>  attrs_;
};
