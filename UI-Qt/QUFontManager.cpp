#if UIQT_USE_FONTCONFIG
#  include <fontconfig/fontconfig.h>
#endif

#include <QCoreApplication>
#include <QLabel>
#include <QStandardPaths>
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

    QLabel * label = new QLabel(nullptr, Qt::FramelessWindowHint);
    label->setFrameStyle(QFrame::Panel | QFrame::Sunken);
    label->setMinimumSize(300, 50);
    db_.reset(new FXFaceDatabase(dirs,
                                 folder.filePath("FontViewer.db").toUtf8().constData(),
                                 [this, label](size_t current, size_t total, const FXString & file) {
                                     label->setText(QString("%1/%2 %3").arg(current).arg(total).arg(toQString(file)));
                                     label->show();
                                     qApp->processEvents();
                                     return true;
                                 }
                  ));
    label->deleteLater();
}

