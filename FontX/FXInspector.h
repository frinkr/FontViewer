#pragma once
#include "FX.h"
#include "FXOT.h"

class FXFace;
struct FXInspectorImp;

struct FXInsOTLookup {
    int index;
};

struct FXInsOTFeature {
    FXTag tag;
    int   index;
    bool  isRequired;
    FXVector<FXInsOTLookup> lookups;
};

struct FXInsOTLanguage {
    FXTag tag;
    int   index;
    FXVector<FXInsOTFeature> features;
};

struct FXInsOTScript {
    FXTag tag;
    int   index;
    FXVector<FXInsOTLanguage>  languages;    
};

struct FXInsOTTable {
    FXTag tag;
    FXVector<FXInsOTScript> scripts;
    FXVector<FXTag> featureTags;
};

class FXInspector {
    
public:
    explicit FXInspector(FXFace * face);

    FXVector<FXTag>
    otScripts(FXTag table = FXOT::MERGED_GSUBGPOS) const;

    FXVector<FXTag>
    otLanguages(FXTag script, FXTag table = FXOT::MERGED_GSUBGPOS) const;

    FXVector<FXTag>
    otFeatures(FXTag script, FXTag language, FXTag table = FXOT::MERGED_GSUBGPOS) const;

private:
    void
    init();

    FXPtr<FXInspectorImp>  imp_;
};
