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

struct FXShapingGenralOptions {
    FXShapingDirection direction {FXShapingLTR};
    FXVector<FXTag> onFeatures {};
    FXVector<FXTag> offFeatures {};
    double glyphSpacing {};
    bool forceShapeGIDEncodedText {};
};

struct FXShapingBidiOptions {
    bool bidiActivated {false};
    FXBidiDirection direction {FXBidiDirection::AUTO};
    bool resolveScripts {true};
    bool resolveUnknownScripts {false};    
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
          const FXShapingGenralOptions & opts = FXShapingGenralOptions{},
          const FXShapingBidiOptions & bidiOpts = FXShapingBidiOptions{});

    size_t
    glyphCount() const;

    FXGlyphID
    glyph(size_t index) const;

    FXVec2d<fu>
    advance(size_t index) const;

    FXVec2d<fu>
    offset(size_t index) const;

    size_t
    cluster(size_t index) const;
    
    FXFace *
    face() const;

    bool
    hasFallbackShaping() const;
private:
    FXPtr<FXShaperImp>  imp_;
};
