#include "FXFace.h"
#include "FXGlyph.h"

FXPixmapARGB
FXGlyph::pixmap() const {
    return face->pixmap(gid);
}
