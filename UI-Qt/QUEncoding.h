#pragma once
#include <QString>
#include "FontX/FX.h"

struct QUEncoding {
    static QString
    charHexNotation(FXChar c, bool unicode = true);
    
};
