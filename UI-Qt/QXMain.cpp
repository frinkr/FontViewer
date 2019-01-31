#include <QtGui>

#ifdef Q_OS_MAC
#  include "MacHelper.h"
#else
#  include "QUTheme.h"
#endif

#include "QXApplication.h"
#include "QXDocumentWindowManager.h"
#include "QXMain.h"
#include "QXFontManager.h"
#include "QXTheme.h"

#if QU_MAIN
int main(int argc, char * argv[])
{
    return quMain(argc, argv);
}
#endif

int qxMain(int argc, char *argv[])
{
#ifdef Q_OS_MAC
    MacHelper::installNSApplicationDelegate();
#endif

    Q_INIT_RESOURCE(QXApplication);
    
    QXApplication app(argc, argv);
    app.setWindowIcon(QIcon(":/images/app.png"));
	qxApp = &app;

#if !defined(Q_OS_MAC)
    QUTheme::applyDarkFusion();
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
#ifndef Q_OS_MAC
        // For macOS, it's handled in MacApplicationDelegate
        QUDocumentWindowManager::instance()->doOpenFontDialog();
#endif
    }  

    return app.exec();
}
