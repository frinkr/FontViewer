#pragma once
#include "FX.h"

#include <boost/predef.h>
#include <boost/range/algorithm/binary_search.hpp>
#include <boost/algorithm/string.hpp>
#include <boost/algorithm/string/split.hpp>
#include <boost/algorithm/string/regex.hpp>

#include <boost/filesystem.hpp>
#include <boost/filesystem/detail/utf8_codecvt_facet.hpp>

#include <filesystem>

namespace BST {
    namespace fs = std::filesystem;
    extern boost::filesystem::detail::utf8_codecvt_facet UTF8Cvt;

    FXString
    pathJoin(const FXString & path, const FXString & name);
    
    FXString
    pathJoin(std::initializer_list<FXString> list);

    FXString &
    pathConcat(FXString & path, const FXString & name);

    FXString &
    pathConcat(FXString & path, std::initializer_list<FXString> list);

    FXString
    fileName(const FXString & path);
    
    bool
    foreachFile(const FXString & directory,
                bool recursive,
                std::function<bool(const FXString &)> handler);

    std::time_t
    lastWriteTime(const FXString & file);

    size_t
    fileSize(const FXString & file);

    FXString
    trim(const FXString & str);
}




