#pragma once
#include "FX.h"
#include "FXIterator.h"
#include <filesystem>

namespace FXFS {
    namespace fs = std::filesystem;

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
}

class FXFileInDirectoryIterator : public FXContainerIterator<FXVector, FXString> {
public:
    FXFileInDirectoryIterator(const FXString & dir, bool recursive = true) {
        FXVector<FXString> files;
        FXFS::foreachFile(dir, recursive, [&files](const auto & file) {
            files.push_back(file);
            return true;
        });
        setList(std::move(files));
    }
};




