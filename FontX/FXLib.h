#pragma once
#include "FX.h"

class FXLib {
public:
    static bool
    init(const std::string & ucdRoot);

    static void
    finish();

    static FXFTLibrary
    get();

private:
    static FXPtr<FXLib> lib_;

public:
    ~FXLib();
private:
    FXLib(const std::string & ucdRoot);
    
    FXFTLibrary     ftlib_;
};


