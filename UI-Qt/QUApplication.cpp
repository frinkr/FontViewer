/*
  Permission for the use of this code is granted only for research, educational, and non-commercial purposes.

  Redistribution of this code or its parts in source, binary, and any other form without permission, with or without modification, is prohibited.  Modifications include, but are not limited to, translation to other programming languages and reuse of tables, constant definitions, and API's defined in it.

  Andrew Choi is not liable for any losses or damages caused by the use of this code.

  Copyright 2009 Andrew Choi.
*/

#include <QtGui>
#include <QtDebug>
#include <QMessageBox>

#include "qudocumentwindowmanager.h"

#include "quapplication.h"

// The QtApplication virtual function commitData doesn't seem to be called on Mac.  So a way to handle an external request to terminate this application seems to be to catch the Apple event kAEQuitApplication.  A little Carbon code.  No big deal.

#ifdef Q_OS_MAC
const QEvent::Type kMacQuitEventType = QEvent::Type(QEvent::registerEventType());
#endif

QUApplication::QUApplication(int &argc, char **argv) : QUSingleApplication(argc, argv)
{
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

// Does not seem to be called on Mac.
#ifndef Q_OS_MAC
void QUApplication::commitData(QSessionManager &sm)
{
    int modifiedDocCount = QUDocumentWindowManager::instance()->countModifiedDocs();

    if (modifiedDocCount == 0)
        return;

    if (sm.allowsInteraction())
    {
        QMessageBox logoutMessageBox(0);

        logoutMessageBox.setIcon(QMessageBox::Warning);
        logoutMessageBox.setText(tr("You have %1 QtDocBasedApp documents with unsaved changes. Do you want to cancel logout so that you can have a chance to first save them?").arg(modifiedDocCount));
        logoutMessageBox.setInformativeText(tr("If you click “Discard Changes and Logout”, all your changes will be lost."));
        logoutMessageBox.setDefaultButton(logoutMessageBox.addButton(tr("Cancel Logout"), QMessageBox::AcceptRole));
        logoutMessageBox.addButton(tr("Discard Changes and Logout"), QMessageBox::DestructiveRole);

        logoutMessageBox.exec();

        switch(logoutMessageBox.buttonRole(logoutMessageBox.clickedButton()))
        {
        case QMessageBox::DestructiveRole:
            sm.release();
            QUDocumentWindowManager::instance()->forceCloseAllDocs();
            break;
        case QMessageBox::AcceptRole:
        default:
            sm.cancel();
        }
    }
}
#endif
