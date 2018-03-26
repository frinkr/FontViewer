/*
  Permission for the use of this code is granted only for research, educational, and non-commercial purposes.

  Redistribution of this code or its parts in source, binary, and any other form without permission, with or without modification, is prohibited.  Modifications include, but are not limited to, translation to other programming languages and reuse of tables, constant definitions, and API's defined in it.

  Andrew Choi is not liable for any losses or damages caused by the use of this code.

  Copyright 2009 Andrew Choi.
*/

#include <QtGui>
#include <QtDebug>
#include <QMessageBox>

#include "QUDocumentWindowManager.h"
#include "QUApplication.h"

QUApplication::QUApplication(int &argc, char **argv) : QUSingleApplication(argc, argv)
{
    setOrganizationName("DANIEL JIANG");
    setOrganizationDomain("frinkr.top");
    setApplicationName("FontViewer");
    
#ifdef Q_OS_MAC
    setQuitOnLastWindowClosed(false);
#endif
}

QUApplication::~QUApplication()
{
}

#ifdef Q_OS_MAC
// This opens a document using this application from the Finder.
bool QUApplication::event(QEvent *event)
{
    if (event->type() == QEvent::FileOpen)
    {
        QUDocumentWindowManager::instance()->openFile(static_cast<QFileOpenEvent *>(event)->file());

        return true;
    }

    return QApplication::event(event);
}
#endif
