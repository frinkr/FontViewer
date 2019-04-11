#include <Shlobj.h>
#include "WinHelper.h"

void
WinHelper::showInExplorer(const QString & path) {
    ITEMIDLIST *pidl = ILCreateFromPath(path.toStdWString().c_str());
    if(pidl) {
        SHOpenFolderAndSelectItems(pidl,0,0,0);
        ILFree(pidl);
    }
}
