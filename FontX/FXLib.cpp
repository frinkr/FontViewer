#include "FXLib.h"
#include "FXPrivate.h"

namespace {
    TXFTLibrary lib_;
}

bool
FXLib::init() {
    if (lib_)
        return true;
    
    if (FT_Init_FreeType(&lib_)) {
        lib_ = nullptr;
        return false;
    }
    return true;
}

bool
FXLib::finish() {
    if (!lib_)
        return true;

    if (!FT_Done_Library(lib_)) {
        lib_ = nullptr;
        return true;
    }
    return false;
}

TXFTLibrary
FXLib::get() {
    return lib_;
}
