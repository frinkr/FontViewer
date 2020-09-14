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

    // Left bottom offset to draw the image in an EM square
    FXVec2d<int>     offset {};
    
    // The bitmap scale related to emSize.
    // pixmap and offset should be scaled to be painted correctly
    double           scale {1};

    // The EM square
    FXVec2d<int>     emSize {};
    
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
