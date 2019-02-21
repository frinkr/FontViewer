#pragma once
#include <functional>
#include <iomanip>
#include <sstream>

#include "FX.h"

enum class FXLogLevel : int {
    error,
    warning,
    info,
    verboseInfo,
};

struct FXLogLocation {
    const char * file;
    size_t       line;
    const char * func;
};
    
using FXLogFunc = std::function<void(FXLogLevel level, const FXLogLocation * location, const char * message)>;

void
FXDefaultLogFunc(FXLogLevel level, const FXLogLocation * location, const char * message);

FXLogFunc
FXGetLogFunc();

void
FXSetLogFunc(FXLogFunc func);


/////////////////////////////////////////////////////////////////

#define FX_DO_LOG 1
#if FX_DO_LOG

#if __GNUC__ >= 3
#  define FX_FUNC __PRETTY_FUNCTION__
#elif defined(_MSC_VER)
#  define FX_FUNC __FUNCSIG__
#else
#  define FX_FUNC __func__
#endif

#define FX_LOG_LEVEL(level, ...)                            \
    do {                                                    \
        if (!FXGetLogFunc())                                \
            break;                                          \
        std::stringstream __fx_log_level_ss__;              \
        __fx_log_level_ss__                                 \
            << std::fixed                                   \
            << std::setprecision(8)                         \
            << __VA_ARGS__ << std::endl;                    \
        FXLogLocation loc;                                  \
        loc.file = __FILE__;                                \
        loc.line = __LINE__;                                \
        loc.func = FX_FUNC;                                 \
        FXGetLogFunc()(level,                               \
                       &loc,                                \
                       __fx_log_level_ss__.str().c_str());  \
    } while(false)
    
#else
#define FX_LOG_LEVEL(level, ...)
#endif

#define FX_ERROR(...)        FX_LOG_LEVEL(FXLogLevel::error,   __VA_ARGS__)
#define FX_WARNING(...)      FX_LOG_LEVEL(FXLogLevel::warning, __VA_ARGS__)
#define FX_INFO(...)         FX_LOG_LEVEL(FXLogLevel::info,    __VA_ARGS__)
#define FX_VERBOSE_INFO(...) FX_LOG_LEVEL(FXLogLevel::verboseInfo, __VA_ARGS__)
