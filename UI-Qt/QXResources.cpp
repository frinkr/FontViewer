#include "QXResources.h"
#include <QCoreApplication>
#include <QDir>

namespace QXResources {
    QString
    path(const QString & name) {
#ifdef Q_OS_MAC
        return QDir::toNativeSeparators(QDir(qApp->applicationDirPath()).absoluteFilePath("../Resources/" + name));
#else
        return QDir::toNativeSeparators(QDir(qApp->applicationDirPath()).absoluteFilePath("Resources/" + name));
#endif
    }
}
