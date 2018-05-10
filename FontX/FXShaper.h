#pragma once
#include "FX.h"
#include "FXTag.h"

class FXFace;
struct FXShaperImp;

enum FXShapingDirection {
    FXShappingDirectionInvalid = 0,
    FXShappingDirectionAuto,
    FXShappingLTR = 4,
    FXShappingRTL,
    FXShappingTTB,
    FXShappingBTT
};

class FXShaper {
public:
    explicit FXShaper(FXFace * face);
    
    void
    shape(const FXString & text,
          FXTag script,
          FXTag language,
          FXShapingDirection direction,
          const FXVector<FXTag> & onFeatures,
          const FXVector<FXTag> & offFeatures = FXVector<FXTag>());

    size_t
    glyphCount() const;

    FXGlyphID
    glyph(size_t index) const;

    FXVec2d<fu>
    advance(size_t index) const;

    FXVec2d<fu>
    offset(size_t index) const;

private:
    FXPtr<FXShaperImp>  imp_;
};
