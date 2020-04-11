#include "FXLib.h"
#include "FXVersion.h"
#include "FXFTPrivate.h"
#include "FXHBPrivate.h"
#include "unicode/uversion.h"

FXString
FXVersion::freetype() {
    FT_Int major, minor, patch;
    FT_Library_Version(FXLib::get(), &major, &minor, &patch);
    return
        std::to_string(major) + "." +
        std::to_string(minor) + "." +
        std::to_string(patch);
}

FXString
FXVersion::harfbuzz() {
    return hb_version_string();
}

FXString
FXVersion::icu() {
    return
        std::to_string(U_ICU_VERSION_MAJOR_NUM) + "." +
        std::to_string(U_ICU_VERSION_MINOR_NUM) + "." +
        std::to_string(U_ICU_VERSION_PATCHLEVEL_NUM) + ", Build " +
        std::to_string(U_ICU_VERSION_BUILDLEVEL_NUM) + ", Data " +
        U_ICU_DATA_VERSION;
}
