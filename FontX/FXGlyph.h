#pragma once
#include <variant>
#include "FX.h"
#include "FXPixmap.h"

class FXFace;

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


struct FXGlyphImage {
    enum Mode {
        kColor,
        kGrayscale,
        kMono,
    };

    FXPixmapARGB     pixmap {};
    Mode             mode {kGrayscale};
    FXVec2d<int>     offset {};
    FXVec2d<int>     emSize {};
    
    bool empty() const {
        return pixmap.empty();
    }

    int width() const {
        return pixmap.width;
    }

    int height() const {
        return pixmap.height;
    }

    bool isSmoothScalable() const {
        return mode != kMono;
    }
};

struct FXGlyph {
    FXFace          * face;
    FXGlyphID         gid;
    FXString          name;    
    FXGChar           character; // isChar() returns true
    FXGlyphMetrics    metrics;
    
    FXGlyphImage
    glyphImage() const;
};
