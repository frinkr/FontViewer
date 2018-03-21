#include <ft2build.h>
#include FT_FREETYPE_H
#include "FVFace.h"

FVFace::FVFace() {
}

FVFace::FVFace(const FVFaceDescriptor & descriptor)
: iDescriptor(descriptor) {
}