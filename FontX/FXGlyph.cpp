#include "FXFace.h"
#include "FXGlyph.h"

FXGlyphImage
FXGlyph::glyphImage() const {
    return face->glyphImage(gid);
}
