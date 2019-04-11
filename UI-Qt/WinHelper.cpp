#include <Shlobj.h>
#include <QDir>
#include <QFileInfo>
#include <QString>
#include <QProcess>
#include "WinHelper.h"

void
WinHelper::showInExplorer(const QString & path) {
    ITEMIDLIST *pidl = ILCreateFromPathW(QDir::toNativeSeparators(path).toStdWString().c_str());
    if(pidl) {
        SHOpenFolderAndSelectItems(pidl,0,0,0);
        ILFree(pidl);
    }
}
