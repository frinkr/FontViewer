#pragma once
#include "FX.h"

#include <string>


struct FVFaceDescriptor {
    std::string  filePath;
    size_t       faceIndex;
};

class FXFace {
public:
    static FXPtr<FXFace>
    createFace(const FVFaceDescriptor & descriptor);
    
    static FXPtr<FXFace>
    createFace(const std::string & filePath, size_t faceIndex);
    
public:
    TXFTFace
    face() const;
    
    size_t
    index() const;

    std::string
    postscriptName() const;
    
private:
    FXFace(const FVFaceDescriptor & descriptor);
    
    FXFace(const FXFace &) = delete;
    FXFace & operator=(const FXFace & ) = delete;

    bool
    init();
protected:
    FVFaceDescriptor descriptor_;
    TXFTFace         face_;
};
