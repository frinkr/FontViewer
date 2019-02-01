#include <QtGui>

#ifndef Q_OS_MAC
#  include "QXTheme.h"
#endif

#include "QXApplication.h"
#include "QXDocumentWindowManager.h"
#include "QXMain.h"
#include "QXFontManager.h"
#include "QXTheme.h"

#ifdef QU_MAIN
int main(int argc, char * argv[])
{
    return quMain(argc, argv);
}
#endif

int qxMain(int argc, char *argv[])
{
    Q_INIT_RESOURCE(QXApplication);
    
    QXApplication app(argc, argv);
    app.setWindowIcon(QIcon(":/images/app.png"));

#if !defined(Q_OS_MAC)
    QXTheme::setCurrent("Dark Fusion");
    QXTheme::current()->applyToApplication();
#endif
    
    app.setAttribute(Qt::AA_UseHighDpiPixmaps);

    if (QXFontManager::checkResetDatabase())
        QXFontManager::resetDatabase();

    QStringList arguments = app.arguments();
    if (arguments.count() > 1) {
        for (int i = 1; i < arguments.count(); i++)
            QXDocumentWindowManager::instance()->openFontFile(arguments.at(i));
    }
    else {
        QXDocumentWindowManager::instance()->autoOpenFontDialog();
    }  

    return app.exec();
}
