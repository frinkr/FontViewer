#include <boost/foreach.hpp>
#include "FXBoostPrivate.h"

namespace BFS {
    fs::detail::utf8_codecvt_facet UTF8Cvt;

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

        fs::path p(path, UTF8Cvt);
        for (const auto & n : list)
            p /= fs::path(n, UTF8Cvt);

        path = p.string(UTF8Cvt);
        return path;
    }

    FXString
    fileName(const FXString & path) {
        fs::path p(path, UTF8Cvt);
        return p.filename().string(UTF8Cvt);
    }
    
    bool
    foreachFile(const FXString & directory,
                bool recursive,
                std::function<bool(const FXString &)> handler) {
        if (directory.empty())
            return true;
        try {
            fs::path dir(directory, UTF8Cvt); 
            fs::directory_iterator it(dir), eod;
            BOOST_FOREACH(fs::path const & p, std::make_pair(it, eod)) {
                if (recursive && fs::is_directory(p)) {
                    if (foreachFile(p.string(UTF8Cvt), recursive, handler))
                        continue;
                    else
                        return false;
                }
                if (!handler(p.string(UTF8Cvt)))
                    return false;
            }
        }
        catch(...) {
            return false;
        }
        return true;
    }
    
    size_t
    fileSize(const FXString & file) {
        return fs::file_size(fs::path(file, UTF8Cvt));
    }
}
