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

}
