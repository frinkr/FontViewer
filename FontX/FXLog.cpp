#include "FXLog.h"

namespace {
    FXLogFunc s_log_func;
}

void
FXDefaultLogFunc(FXLogLevel level, const FXLogLocation * location, const char * message) {

    const char * s[4] = {"error", "warning", "info", "info"};
    
        printf("TX (%s) : %s", s[(size_t)level], message);

}

FXLogFunc
FXGetLogFunc() {
    if (!s_log_func) s_log_func = FXDefaultLogFunc;
    return s_log_func;
}

void
FXSetLogFunc(FXLogFunc func) {
    s_log_func = func;
}

