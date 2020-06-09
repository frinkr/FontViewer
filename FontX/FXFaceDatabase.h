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

    FXOpt<size_t>
    findIndex(const FXString & psName) const;

    FXOpt<FXFaceDescriptor>
    findDescriptor(const FXString & psName) const;

    void
    rescan();

public:
    struct FaceItem {
        FXFaceDescriptor desc;
        FXFaceAttributes atts;   
    };

    struct FoldersHash {
        size_t   hash {0};

        // map from path to hash
        FXMap<FXString, size_t> files;

        // map from path to indexes of FaceItem in faces_
        // NOT saved to file
        FXMap<FXString, FXVector<size_t>> faces;

    };

protected:
    bool
    save();
    
    bool
    load();

    bool
    checkForUpdate();

    void
    initDiskHash();

protected:
    FXVector<FXString> folders_;
    FXString           dbPath_;

    FXVector<FaceItem> faces_;

    ProgressCallback   progress_;
    FoldersHash        dbHash_;
    FoldersHash        diskHash_;
};
