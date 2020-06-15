#include <QStandardPaths>
#include <QDir>

#include "FontX/FXLog.h"
#include "FCL.h"
#include "FCLDatabase.h"

namespace {
    FXVector<FXString> systemFontFolders() {
        QStringList folders = QStandardPaths::standardLocations(QStandardPaths::FontsLocation);
#if defined(Q_OS_MAC)
        if (!folders.contains("/System/Library/Assets") && !folders.contains("/System/Library/Assets/"))
            folders << "/System/Library/Assets";
        if (!folders.contains("/System/Library/AssetsV2") && !folders.contains("/System/Library/AssetsV2/"))
            folders << "/System/Library/AssetsV2";
#endif
        
        FXVector<FXString> dirs;
        for (const auto & dir : folders)
            dirs.push_back(toStdString(QDir::toNativeSeparators(dir)));

        return dirs;
    }

    FXString dbFilePath() {
        QDir folder(QDir(QStandardPaths::writableLocation(QStandardPaths::AppDataLocation)));
        if (!folder.exists())
            folder.mkpath(".");
        return toStdString(folder.filePath("FontXCommandLine.db"));
    }
}
    
FXPtr<const FCLDatabase>
FCLDatabase::instance() {
    static auto inst = std::make_shared<FCLDatabase>(systemFontFolders(), dbFilePath(), [](size_t current, size_t total, const FXString& file) {
        //FX_VERBOSE_INFO(current << "/" << total << ": caching " << file);
        return true;
    });;
    return inst;
}


namespace {
    FXVector<FXPtr<FCLDatabaseProcessor>> sDbProcessors;
}

const FXVector<FXPtr<FCLDatabaseProcessor>> &
FCLGetDatabaseProcessors() {
    return sDbProcessors;
}

void
FCLAddDatabaseProcessors(FXPtr<FCLDatabaseProcessor> processor) {
    sDbProcessors.push_back(processor);
}


FXPtr<FCLDatabaseProcessor>
FCLFindDatabaseProcessors(const FXString & name) {
    for (auto proc : sDbProcessors) {
        if (proc->name() == name)
            return proc;
    }
    return nullptr;
}
