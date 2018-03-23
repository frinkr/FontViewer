/*
  Permission for the use of this code is granted only for research, educational, and non-commercial purposes.

  Redistribution of this code or its parts in source, binary, and any other form without permission, with or without modification, is prohibited.  Modifications include, but are not limited to, translation to other programming languages and reuse of tables, constant definitions, and API's defined in it.

  Andrew Choi is not liable for any losses or damages caused by the use of this code.

  Copyright 2009 Andrew Choi.
*/

#include <QtGui>

#ifdef Q_OS_MAC
#import "MacApplicationDelegate.h"
#endif

#include "QUApplication.h"
#include "QUDocumentWindowManager.h"
#include "QUMain.h"

#if QU_MAIN
int main(int argc, char * argv[])
{
    return quMain(argc, argv);
}
#endif

int quMain(int argc, char *argv[])
{
#ifdef Q_OS_MAC
    // Initiate shared application and setup delegate so QApplication constructor (ancestor of QtDocBasedApplication) will pick that up as "reflection delegate" (see src/gui/kernel/qapplication_mac.mm)
    NSAutoreleasePool * pool = [[NSAutoreleasePool alloc] init];
    [[NSApplication sharedApplication] setDelegate:[[MacApplicationDelegate alloc] init]];
    [pool release];
#endif

    // For use of fancy single and double quotes in dialog messages.
    //QTextCodec::setCodecForTr(QTextCodec::codecForName("UTF-8"));

    Q_INIT_RESOURCE(QUApplication);
    
    QUApplication a(argc, argv);

    QStringList arguments = a.arguments();
    bool anotherInstanceExists = false;
    for (int i = 1; i < arguments.count(); i++)
        anotherInstanceExists |= a.sendMessage(arguments.at(i).toUtf8());

    // Assume another instance of this application exists if *any* of the messages sent was handled.
    if (anotherInstanceExists)
        return 0;

    // Otherwise start this instance.
    QObject::connect(&a, SIGNAL(messageReceived(const QString &)), QUDocumentWindowManager::instance(), SLOT(openFile(const QString &)));

    if (arguments.count() > 1)
    {
        for (int i = 1; i < arguments.count(); i++)
            QUDocumentWindowManager::instance()->openFile(arguments.at(i));
    }
    else
        QUDocumentWindowManager::instance()->slotOpenFont();

    return a.exec();
}
