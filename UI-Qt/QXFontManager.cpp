#if UIQT_USE_FONTCONFIG
#  include <fontconfig/fontconfig.h>
#endif


#include <QDir>
#include <QLabel>
#include <QMessageBox>
#include <QProgressDialog>
#include <QStandardPaths>

#include "QXConv.h"
#include "QXFontManager.h"
#include "QXPreferences.h"
#include "QXProgressDialog.h"

namespace {
    QStringList _systemFontFolders() {
#if UIQT_USE_FONTCONFIG
        FcConfig * fc = FcInitLoadConfig();
        FcStrList * fcDirs = FcConfigGetFontDirs(fc);
        const FcChar8 * dir = nullptr;
        QStringList ret;
        while ((dir = FcStrListNext(fcDirs))) 
            ret.append(toQString(std::string(reinterpret_cast<const char*>(dir))));
        FcStrListDone(fcDirs);
        FcConfigDestroy(fc);
        return ret;
#else
        QStringList folders = QStandardPaths::standardLocations(QStandardPaths::FontsLocation);
#if defined(Q_OS_MAC)
        if (!folders.contains("/System/Library/Assets") && !folders.contains("/System/Library/Assets/"))
            folders << "/System/Library/Assets";
#endif
        return folders;
#endif
    }

    QXProgressDialog *
	createProgressDialog() {
		QXProgressDialog * progress = new QXProgressDialog(nullptr);
		progress->setWindowModality(Qt::WindowModal);
		progress->setWindowTitle("Rebuilding font database...");
		return progress;
	}

}

void
QXFontManager::resetDatabase() {
    QFile dbFile(dbFilePath());
    if (dbFile.exists())
        dbFile.remove();
    instance();
}

QString
QXFontManager::dbFilePath() {
    QDir folder(QDir(QStandardPaths::writableLocation(QStandardPaths::AppDataLocation)));
    if (!folder.exists())
        folder.mkpath(".");
	return folder.filePath("FontViewer.db");
}


QXFontManager &
QXFontManager::instance() {
    static QXFontManager inst;
    return inst;
}

FXPtr<FXFaceDatabase>
QXFontManager::db() const {
    return db_;
}

const QStringList &
QXFontManager::systemFontFolders() const {
    return systemFontFolders_;
}

const QStringList &
QXFontManager::userFontFolders() const {
    return userFontFolders_;
}

void
QXFontManager::setUserFontFolders(const QStringList & folders) {
    QXPreferences::setUserFontFolders(folders);
    userFontFolders_ = folders;
}

QXFontManager::QXFontManager() {
    userFontFolders_ = QXPreferences::userFontFolders();
    systemFontFolders_ = _systemFontFolders();
    FXVector<FXString> dirs;
    for (const auto & dir : systemFontFolders_)
        dirs.push_back(toStdString(QDir::toNativeSeparators(dir)));
    for (const auto & dir : userFontFolders_)
        dirs.push_back(toStdString(QDir::toNativeSeparators(dir)));

	const QString dbPath = dbFilePath();

	// Load database.
	QXProgressDialog * progress = createProgressDialog();
    db_.reset(new FXFaceDatabase(dirs,			
		dbPath.toUtf8().constData(),
        [progress](size_t current, size_t total, const FXString & file) {
            progress->setProgress(static_cast<int>(current), static_cast<int>(total), toQString(file));
            return true;
        }
    ));
    progress->close();
	delete progress;
}

