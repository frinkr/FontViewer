#if UIQT_USE_FONTCONFIG
#  include <fontconfig/fontconfig.h>
#endif


#include <QDir>
#include <QGuiApplication>
#include <QLabel>
#include <QMessageBox>
#include <QProgressDialog>
#include <QStandardPaths>

#include "QXConv.h"
#include "QXFontManager.h"
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
        return QStandardPaths::standardLocations(QStandardPaths::FontsLocation);
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

bool
QXFontManager::checkResetDatabase() {
    bool keyPressed = (qApp->queryKeyboardModifiers() & (Qt::ShiftModifier | Qt::ControlModifier)) == (Qt::ShiftModifier | Qt::ControlModifier);
    QFile dbFile(dbFilePath());
    if (keyPressed && dbFile.exists()) {
        if (QMessageBox::Yes == QMessageBox::question(
                nullptr,
                tr("Reset font database"),
                tr("Are you sure to reset the database? The font database will be deleted and rebuilt."),
                QMessageBox::Yes | QMessageBox::No, QMessageBox::Yes)) {
            return true;
		}
    }
    return false;
}

void
QXFontManager::resetDatabase() {
    QFile dbFile(dbFilePath());
    if (dbFile.exists())
        dbFile.remove();
    get();
}

QString
QXFontManager::dbFilePath() {
    QDir folder(QDir(QStandardPaths::writableLocation(QStandardPaths::AppDataLocation)));
    if (!folder.exists())
        folder.mkpath(".");
	return folder.filePath("FontViewer.db");
}


QXFontManager &
QXFontManager::get() {
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
    userFontFolders_ = folders;
}

QXFontManager::QXFontManager() {
    systemFontFolders_ = _systemFontFolders();
    FXVector<FXString> dirs;
    for (const auto & dir : systemFontFolders_)
        dirs.push_back(QDir::toNativeSeparators(dir).toUtf8().constData());

	const QString dbPath = dbFilePath();

	// Load database.
	QXProgressDialog * progress = createProgressDialog();
    db_.reset(new FXFaceDatabase(dirs,			
		dbPath.toUtf8().constData(),
        [progress](size_t current, size_t total, const FXString & file) {
            progress->setProgress(current, total, toQString(file));
            return true;
        }
    ));
	progress->deleteLater();
}

