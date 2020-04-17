#include <chrono>

#include "FXFS.h"

#if FX_WIN
#  include <Windows.h>
#endif

namespace FXFS {
    FXString
    pathJoin(const FXString & path, const FXString & name) {
        return pathJoin({path, name});
    }
    
    FXString
    pathJoin(std::initializer_list<FXString> list) {
        FXString path;
        return pathConcat(path, list);
    }
    
    FXString &
    pathConcat(FXString & path, const FXString & name) {
        return pathConcat(path, {name});
    }
    
    FXString &
    pathConcat(FXString & path, std::initializer_list<FXString> list) {
        if (!list.size()) return path;

        fs::path p(fs::u8path(path));
        for (const auto & n : list)
            p /= fs::u8path(n);

        path = p.u8string();
        return path;
    }

    FXString
    fileName(const FXString & path) {
        fs::path p(fs::u8path(path));
        return p.filename().u8string();
    }
    
    bool
    foreachFile(const FXString & directory,
                bool recursive,
                std::function<bool(const FXString &)> handler) {
        if (directory.empty())
            return true;
        try {
            const fs::path dir(fs::u8path(directory)); 
            for (auto & ent: fs::directory_iterator(dir)) {
                if (recursive && fs::is_directory(ent)) {
                    if (foreachFile(ent.path().u8string(), recursive, handler))
                        continue;
                    else
                        return false;
                }
                if (!handler(ent.path().u8string()))
                    return false;
            }
        }
        catch(...) {
            return false;
        }
        return true;
    }
    
    std::time_t
    lastWriteTime(const FXString & file) {
        auto time = fs::last_write_time(fs::u8path(file));
#ifdef _MSC_VER
        const FILETIME * ft = reinterpret_cast<const FILETIME*>(&time);
        ULARGE_INTEGER ull;
        ull.LowPart = ft->dwLowDateTime;
        ull.HighPart = ft->dwHighDateTime;
        return ull.QuadPart / 10000000ULL - 11644473600ULL;
#else
        return decltype(time)::clock::to_time_t(time);
#endif
    }

    size_t
    fileSize(const FXString & file) {
        return fs::file_size(fs::u8path(file));
    }
}
