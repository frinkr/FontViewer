#include <QtGui>

#ifdef Q_OS_MAC
#  include "MacHelper.h"
#endif

#include "QUApplication.h"
#include "QUDocumentWindowManager.h"
#include "QUMain.h"
#include "QUFontManager.h"

#if QU_MAIN
int main(int argc, char * argv[])
{
    return quMain(argc, argv);
}
#endif

int quMain(int argc, char *argv[])
{
#ifdef Q_OS_MAC
    MacHelper::installNSApplicationDelegate();
#endif

    Q_INIT_RESOURCE(QUApplication);
    
    QUApplication app(argc, argv);
    app.setWindowIcon(QIcon(":/images/app.png"));
	quApp = &app;

    app.setAttribute(Qt::AA_UseHighDpiPixmaps);

    if (QUFontManager::checkResetDatabase())
        QUFontManager::resetDatabase();

    QStringList arguments = app.arguments();
    if (arguments.count() > 1) {
        for (int i = 1; i < arguments.count(); i++)
            QUDocumentWindowManager::instance()->openFile(arguments.at(i));
    }
    else {
#ifndef Q_OS_MAC
        // For macOS, it's handled in MacApplicationDelegate
        QUDocumentWindowManager::instance()->doOpenFontDialog();
#endif
    }  

    return app.exec();
}
