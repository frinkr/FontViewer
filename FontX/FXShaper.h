#pragma once
#include "FX.h"
#include "FXOT.h"

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
          FXTag script = FXOT::DEFAULT_SCRIPT,
          FXTag language = FXOT::DEFAULT_LANGUAGE,
          FXShapingDirection direction = FXShappingLTR,
          const FXVector<FXTag> & onFeatures = FXVector<FXTag>(),
          const FXVector<FXTag> & offFeatures = FXVector<FXTag>());

    size_t
    glyphCount() const;

    FXGlyphID
    glyph(size_t index) const;

    FXVec2d<fu>
    advance(size_t index) const;

    FXVec2d<fu>
    offset(size_t index) const;

    FXFace *
    face() const;

    bool
    hasFallbackShaping() const;
private:
    FXPtr<FXShaperImp>  imp_;
};
