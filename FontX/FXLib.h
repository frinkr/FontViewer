#pragma once
#include "FX.h"
#include "FXUCD.h"

class FXLib {
public:
    static bool
    init(const std::string & ucdRoot);

    static void
    finish();

    static FXFTLibrary
    get();

    static FXPtr<FXUCD>
    ucd();
private:
    static FXPtr<FXLib> lib_;

public:
    ~FXLib();
private:
    FXLib(const std::string & ucdRoot);
    
    FXFTLibrary     ftlib_;
    FXPtr<FXUCD>    ucd_;
};


