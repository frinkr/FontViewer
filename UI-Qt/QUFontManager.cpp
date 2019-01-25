#if UIQT_USE_FONTCONFIG
#  include <fontconfig/fontconfig.h>
#endif

#include <QGuiApplication>
#include <QLabel>
#include <QStandardPaths>
#include <QMessageBox>
#include <QProgressDialog>
#include <QDir>
#include "QUConv.h"
#include "QUFontManager.h"

namespace {
    QStringList fontDirs() {
#if UIQT_USE_FONTCONFIG
        FcConfig * fc = FcInitLoadConfig();
        FcStrList * fcDirs = FcConfigGetFontDirs(fc);
        const FcChar8 * dir = nullptr;
        QStringList ret;
        while ((dir = FcStrListNext(fcDirs))) 
            ret.append(toQString(std::string((const char*)dir)));
        FcStrListDone(fcDirs);
        FcConfigDestroy(fc);
        return ret;
#else
        return QStandardPaths::standardLocations(QStandardPaths::FontsLocation);
#endif
    }

	bool
	checkResetDatabase() {
		return (qApp->queryKeyboardModifiers() & (Qt::ShiftModifier | Qt::ControlModifier)) == (Qt::ShiftModifier | Qt::ControlModifier);
	}

	QProgressDialog *
	createProgressDialog() {
		QProgressDialog * progress = new QProgressDialog(nullptr, Qt::Dialog);
		progress->setWindowModality(Qt::WindowModal);
		progress->setMinimumWidth(400);
		progress->setMaximumWidth(400);
		progress->setAutoReset(false);
		progress->setCancelButton(nullptr);
		progress->setWindowTitle("Rebuilding font database...");
		return progress;
	}

}

QUFontManager &
QUFontManager::get() {
    static QUFontManager inst;
    return inst;
}

FXPtr<FXFaceDatabase>
QUFontManager::db() const {
    return db_;
}

QUFontManager::QUFontManager() {
    directories_ = fontDirs();
    FXVector<FXString> dirs;
    for (const auto & dir : directories_)
        dirs.push_back(QDir::toNativeSeparators(dir).toUtf8().constData());

    QDir folder(QDir(QStandardPaths::writableLocation(QStandardPaths::AppDataLocation)));
    if (!folder.exists())
        folder.mkpath(".");

	// Check to reset database.
	const QString dbPath = folder.filePath("FontViewer.db");
	QFile dbFile(dbPath);
	if (dbFile.exists() && checkResetDatabase()) {
		if (QMessageBox::Yes == QMessageBox::question(nullptr,
			tr("Reset font database"),
			tr("Are you sure to reset the database? The font database will be deleted and rebuilt."),
			QMessageBox::Yes | QMessageBox::No, QMessageBox::Yes)) {
			dbFile.remove();
		}
	}

	// Load database.
	QProgressDialog * progress = createProgressDialog();
    db_.reset(new FXFaceDatabase(dirs,			
		dbPath.toUtf8().constData(),
        [this, progress](size_t current, size_t total, const FXString & file) {
			progress->setMaximum(total);
			progress->setValue(current);
			progress->setLabelText(toQString(file));
            return true;
        }
    ));
	progress->deleteLater();
}

