#pragma once
#include "FX.h"

struct FXLib {
public:
    static bool
    init();

    static bool
    finish();

    static TXFTLibrary
    get();
};


