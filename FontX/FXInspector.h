#pragma once
#include "FX.h"
#include "FXTag.h"

class FXFace;

struct FXInspectorImp;

class FXInspector {

public:
    explicit FXInspector(FXFace * face);

    const FXVector<FXTag>
    scripts(FXTag table) const;

    const FXVector<FXTag>
    languages(FXTag table, FXTag script) const;

    const FXVector<FXTag>
    features(FXTag table, FXTag script, FXTag language) const;

private:
    void
    init();

    FXPtr<FXInspectorImp>  imp_;
};
