#pragma once
#include "FX.h"
#include "FXBitmap.h"

struct FXGlyphMetrics {
    using value_type = fu;

    value_type        width;
    value_type        height;

    value_type        horiBearingX;
    value_type        horiBearingY;
    value_type        horiAdvance;      // native advance without shaping    
    value_type        vertBearingX;
    value_type        vertBearingY;
    value_type        vertAdvance;

    value_type
    lsb() const {
        return horiBearingX;
    }

    value_type
    rsb() const {
        return horiAdvance - width - horiBearingX;
    }
};

struct FXGlyph {
    FXGlyphID         id;
    FXString          name;    
    FXChar            character;
    FXGlyphMetrics    metrics;
    FXBitmapARGB      bitmap;
};
