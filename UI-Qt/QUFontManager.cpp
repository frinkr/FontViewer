#include <QStandardPaths>
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
    
    db_.reset(new FXFaceDatabase(dirs,
				 QStandardPaths::locate(QStandardPaths::AppDataLocation, "FontViewer.db").toUtf8().constData()));
}

