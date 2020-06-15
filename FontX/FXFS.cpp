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
    
    template <typename TP>
    std::time_t to_time_t(TP tp)
    {
        using namespace std::chrono;
        auto sctp = time_point_cast<system_clock::duration>(tp - TP::clock::now()
                + system_clock::now());
        return system_clock::to_time_t(sctp);
    }

    std::time_t
    lastWriteTime(const FXString & file) {
        return to_time_t(fs::last_write_time(fs::u8path(file)));
    }

    size_t
    fileSize(const FXString & file) {
        return fs::file_size(fs::u8path(file));
    }
}
