#pragma once
#include <string>

struct FVFaceDescriptor {
    std::string  filePath;
    size_t       faceIndex;
};

class FVFace {
public:
    FVFace();
    explicit FVFace(const FVFaceDescriptor & descriptor);

protected:
    FVFaceDescriptor iDescriptor;
};