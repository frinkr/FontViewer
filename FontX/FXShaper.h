#pragma once
#include "FX.h"
#include "FXOT.h"

class FXFace;
struct FXShaperImp;

enum FXShapingDirection {
    FXShapingDirectionInvalid = 0,
    FXShapingDirectionAuto,
    FXShapingLTR = 4,
    FXShapingRTL,
    FXShapingTTB,
    FXShapingBTT
};

enum class FXBidiDirection {
    LTR,
    RTL,
    AUTO,
};

struct FXShapingBidiOptions {
    bool bidiActivated {false};
    FXBidiDirection direction {FXBidiDirection::AUTO};
    bool resolveScripts {true};
    bool breakOnScriptChange {true};
    bool breakOnLevelChange {true};
    
};

class FXShaper {
public:
    explicit FXShaper(FXFace * face);
    
    void
    shape(const FXString & text,
          FXTag script = FXOT::DEFAULT_SCRIPT,
          FXTag language = FXOT::DEFAULT_LANGUAGE,
          FXShapingDirection direction = FXShapingLTR,
          FXShapingBidiOptions bidiOpts = FXShapingBidiOptions{},
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
