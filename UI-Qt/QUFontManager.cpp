#include <QStandardPaths>
#include <QDir>
#include "QUConv.h"
#include "QUFontManager.h"

QUFontManager &
QUFontManager::get() {
    static QUFontManager inst;
    return inst;
}

QUFontManager::QUFontManager() {
    directories_ = QStandardPaths::standardLocations(QStandardPaths::FontsLocation);

    FXVector<FXString> dirs;
    for (const auto & dir : directories_)
	dirs.push_back(dir.toUtf8().constData());

    QDir folder(QDir(QStandardPaths::writableLocation(QStandardPaths::AppDataLocation)));
    if (!folder.exists())
        folder.mkpath(".");
    
    db_.reset(new FXFaceDatabase(dirs,
				 folder.filePath("FontViewer.db").toUtf8().constData()));
}

