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

struct FXShapingOptions {
    FXShapingDirection direction {FXShapingLTR};
    FXVector<FXTag> onFeatures {};
    FXVector<FXTag> offFeatures {};
    double glyphSpacing {};
    bool forceShapeGIDEncodedText {};

    struct Bidi {
        bool activated {false};
        FXBidiDirection direction {FXBidiDirection::AUTO};
        bool resolveScripts {true};
        bool resolveUnknownScripts {false};    
        bool breakOnScriptChange {true};
        bool breakOnLevelChange {true};
    } bidi; 
};



class FXShaper {
public:
    struct GlyphInfo {
        FXGlyphID     id {};
        size_t        cluster {};        
        FXVec2d<fu>   advance {};
        FXVec2d<fu>   offset {};
        bool          rtl {};
        FXVec2d<fu>   spacing {};
        FXVec2d<fu>   kerning {};
    };
public:
    explicit FXShaper(FXFace * face);
    
    void
    shape(const FXString & text,
          FXTag script = FXOT::DEFAULT_SCRIPT,
          FXTag language = FXOT::DEFAULT_LANGUAGE,
          const FXShapingOptions & opts = FXShapingOptions{});


    size_t
    glyphCount() const;

    const GlyphInfo &
    glyphInfo(size_t index) const;
    
    FXGlyphID
    glyph(size_t index) const;

    FXVec2d<fu>
    advance(size_t index) const;

    FXVec2d<fu>
    offset(size_t index) const;

    size_t
    cluster(size_t index) const;

    FXVec2d<fu>
    spacing(size_t index) const;
    
    FXVec2d<fu>
    kerning(size_t index) const;
    
    FXFace *
    face() const;

    bool
    hasFallbackShaping() const;
private:
    FXPtr<FXShaperImp>  imp_;
};
