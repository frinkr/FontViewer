#include "FXFace.h"
#include "FXGlyph.h"

FXBitmapARGB
FXGlyph::bitmap() const {
    return face->glyphImage(gid);
}
