#include "FXLib.h"
#include "FXPrivate.h"

FXPtr<FXLib> FXLib::lib_;

bool
FXLib::init(const std::string & ucdRoot) {
    if (lib_)
        return true;
    
    lib_.reset(new FXLib(ucdRoot));
    return true;
}

void
FXLib::finish() {
    lib_.reset();
}

FXFTLibrary
FXLib::get() {
    return lib_->ftlib_;
}

 FXPtr<FXUCD>
 FXLib::ucd() {
     return lib_->ucd_;
 }

FXLib::FXLib(const std::string & ucdRoot) {
    if (FT_Init_FreeType(&ftlib_)) 
        ftlib_ = nullptr;
    
    if (ftlib_) {
        ucd_.reset(new FXUCD(ucdRoot));
    }
}

FXLib::~FXLib() {
    if (ftlib_ && !FT_Done_Library(ftlib_)) 
        ftlib_ = nullptr;
}

