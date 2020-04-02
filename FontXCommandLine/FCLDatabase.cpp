#include <QStandardPaths>
#include <QDir>

#include "FCL.h"
#include "FCLDatabase.h"

namespace {
    FXVector<FXString> systemFontFolders() {
        QStringList folders = QStandardPaths::standardLocations(QStandardPaths::FontsLocation);
#if defined(Q_OS_MAC)
        if (!folders.contains("/System/Library/Assets") && !folders.contains("/System/Library/Assets/"))
            folders << "/System/Library/Assets";
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
    


FXPtr<FCLDatabase>
FCLDatabase::instance() {
    static auto inst = std::make_shared<FCLDatabase>(systemFontFolders(), dbFilePath());
    return inst;
}
