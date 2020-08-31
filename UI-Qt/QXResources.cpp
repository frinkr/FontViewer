#include "QXResources.h"
#include <QCoreApplication>
#include <QDir>

namespace QXResources {
    QString
    path(const QString & name) {
        return QDir::toNativeSeparators(QDir(qApp->applicationDirPath()).absoluteFilePath("Resources/" + name));
    }
}
