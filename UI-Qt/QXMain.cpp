#include <QtGui>

#ifdef Q_OS_MAC
#  include "MacHelper.h"
#else
#  include "QXTheme.h"
#endif

#include "QXApplication.h"
#include "QXDocument.h"
#include "QXDocumentWindowManager.h"
#include "QXFontManager.h"
#include "QXMain.h"
#include "QXPreferences.h"
#include "QXTheme.h"

#ifdef QX_MAIN
int main(int argc, char * argv[]) {
    return qxMain(argc, argv);
}
#endif

int qxMain(int argc, char *argv[]) {
#if defined(Q_OS_MAC)
    MacHelper::installNSApplicationDelegate();
#endif
    Q_INIT_RESOURCE(QXApplication);
    
    QXApplication app(argc, argv);
    app.setWindowIcon(QIcon(":/images/app.png"));

    QXPreferences::load();

    const bool resetAppData = app.userRequiredToResetAppData();
    if (resetAppData)
        QXPreferences::reset();


    QXTheme::setCurrent(QXPreferences::theme());
    app.setAttribute(Qt::AA_UseHighDpiPixmaps);
    app.showSplashScreen();
    app.processEvents();
    if (resetAppData) 
        QXFontManager::resetDatabase();

    QStringList arguments = app.arguments();
    if (arguments.count() > 1) {
        for (int i = 1; i < arguments.count(); i++) {
            auto uri = QXFontURI::fromString(arguments.at(i));
            if (uri.faceIndex == -1)
                QXDocumentWindowManager::instance()->openFontFile(uri.filePath);
            else 
                QXDocumentWindowManager::instance()->openFontURI(uri);
        }
    }
    else {
#if !defined(Q_OS_MAC)
        QXDocumentWindowManager::instance()->autoOpenFontDialog();
#endif
    }  

    int code = app.exec();
    QXPreferences::save();
    return code;
}
