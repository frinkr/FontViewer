#pragma

#include "FXTag.h"

namespace FXOT {
    extern const FXTag DEFAULT_SCRIPT;
    extern const FXTag DEFAULT_LANGUAGE;
    extern const FXTag MERGED_GSUBGPOS;
    
    extern const unsigned int DEFAULT_LANGUAGE_INDEX;
    
    FXString
    scriptName(FXTag script);

    FXString
    languageName(FXTag language);

    FXString
    featureName(FXTag feature);
}
