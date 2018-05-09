#pragma once
#include "FX.h"
#include "FXTag.h"

class FXFace;

class FXShaper {

    void
    shape(const FXString & text, FXTag script, FXTag language, const FXVector<FXTag> & features);

    size_t
    glyphCount() const;

    FXGlyphID
    glyph(size_t index) const;

    FXVec2d<fu>
    offset(size_t index) const;

    FXVec2d<fu>
    advance(size_t index) const;
};
