/*
  Permission for the use of this code is granted only for research, educational, and non-commercial purposes.

  Redistribution of this code or its parts in source, binary, and any other form without permission, with or without modification, is prohibited.  Modifications include, but are not limited to, translation to other programming languages and reuse of tables, constant definitions, and API's defined in it.

  Andrew Choi is not liable for any losses or damages caused by the use of this code.

  Copyright 2009 Andrew Choi.
*/

#include <QtGui>
#include <QtDebug>
#include <QMenu>
#include <QMenuBar>
#include <QDesktopWidget>
#include <QApplication>
#include <QMessageBox>
#include <QFileDialog>

#ifdef Q_OS_MAC
#  import <Cocoa/Cocoa.h>
#  import "ApplicationDelegate.h"
#endif

#include "qudocumentwindow.h"

#include "qudocumentwindowmanager.h"

QUDocumentWindowManager *QUDocumentWindowManager::_instance = 0;

QUDocumentWindowManager::QUDocumentWindowManager()
{
    QCoreApplication::setOrganizationName("Andrew Choi");
    QCoreApplication::setOrganizationDomain("sixthhappiness.ca");
    QCoreApplication::setApplicationName("QtDocBasedApp");

    QSettings settings;
    recentFiles = settings.value("recentFiles").toStringList();

    noDocOpened = true;  // True only before any document window has been opened.
    onlyFirstUntitledDocOpened = false;  // True only during the time the first untitled document window exists and is the only window.

    quitPending = false;

#ifdef Q_OS_MAC
    // Create default menu bar.
    QMenuBar *mb = new QMenuBar;

    QMenu *fileMenu = mb->addMenu(tr("&File"));
    fileMenu->addAction(tr("&New"), this, SLOT(newFile()), QKeySequence(Qt::CTRL | Qt::Key_N));
    fileMenu->addAction(tr("&Open..."), this, SLOT(open()), QKeySequence(Qt::CTRL | Qt::Key_O));

    openRecentSubMenu = fileMenu->addMenu(tr("Open Recent"));
    connect(fileMenu, SIGNAL(aboutToShow()), this, SLOT(slotAboutToShowFileMenu()));
    slotAboutToShowFileMenu();

    fileMenu->addSeparator();
    QAction *a = fileMenu->addAction(tr("Close"));
    a->setShortcut(tr("Ctrl+W"));
    a->setEnabled(false);
    a = fileMenu->addAction(tr("Save"));
    a->setEnabled(false);
    a->setShortcut(tr("Ctrl+S"));
    a = fileMenu->addAction(tr("Save As..."));
    a->setEnabled(false);
    a->setShortcut(tr("Ctrl+Shift+S"));

    fileMenu->addAction(tr("&Quit"), this, SLOT(closeDocumentsAndQuit()), QKeySequence(Qt::CTRL | Qt::Key_Q));

    QMenu *editMenu = mb->addMenu(tr("Edit"));
    a = editMenu->addAction(tr("Undo"));
    a->setEnabled(false);
    a->setShortcut(tr("Ctrl+Z"));
    a = editMenu->addAction(tr("Redo"));
    a->setEnabled(false);
    a->setShortcut(tr("Ctrl+Shift+Z"));
    editMenu->addSeparator();
    a = editMenu->addAction(tr("Cut"));
    a->setEnabled(false);
    a->setShortcut(tr("Ctrl+X"));
    a = editMenu->addAction(tr("Copy"));
    a->setEnabled(false);
    a->setShortcut(tr("Ctrl+C"));
    a = editMenu->addAction(tr("Paste"));
    a->setEnabled(false);
    a->setShortcut(tr("Ctrl+V"));
    a = editMenu->addAction(tr("Delete"));
    a->setEnabled(false);
    a = editMenu->addAction(tr("Select All"));
    a->setEnabled(false);
    a->setShortcut(tr("Ctrl+A"));

    QMenu *windowMenu = mb->addMenu(tr("Window"));
    a = windowMenu->addAction(tr("Minimize"));
    a->setEnabled(false);
    a->setShortcut(tr("Ctrl+M"));
    a = windowMenu->addAction(tr("Zoom"));
    a->setEnabled(false);
    windowMenu->addSeparator();
    a = windowMenu->addAction(tr("Bring All To Front"));
    a->setEnabled(false);

    QMenu *helpMenu = mb->addMenu(tr("Help"));
    helpMenu->addAction(tr("&About QtDocBasedApp"), this, SLOT(about()));
    helpMenu->addAction(tr("QtDocBasedApp &Help"), this, SLOT(help()));
#endif

    connect(qApp, SIGNAL(aboutToQuit()), this, SLOT(saveRecentFilesSettings()));
}

// Singleton
QUDocumentWindowManager *QUDocumentWindowManager::instance()
{
    if (_instance == 0)
    {
        _instance = new QUDocumentWindowManager;
    }

    return _instance;
}

void QUDocumentWindowManager::removeDocumentWindow(QUDocumentWindow *w)
{
    for (int i = windows.count() - 1; i >= 0; --i)
        if (windows.at(i) == w)
            windows.removeAt(i);
}

// Qt doesn't seem to allow Z order of windows to be determined so we keep track of it ourselves (activeWindow() only works when application is running in foreground).
bool QUDocumentWindowManager::eventFilter(QObject *watched, QEvent *event)
{
    if (event->type() == QEvent::WindowActivate)
    {
        QUDocumentWindow *w =  qobject_cast<QUDocumentWindow *>(watched);

        windows.removeAll(w);
        windows.prepend(w);
    }

    return false;
}

void QUDocumentWindowManager::addRecentFilesMenuActions(QMenu *recentFilesMenu)
{
    removeNonExistingRecentFiles();
    QStringList displayNames = recentFileDisplayNames();

#ifndef Q_OS_MAC
    for (int i = 0; i < recentFiles.count(); i++)
        displayNames[i] = QString("&%1. %2").arg(i + 1).arg(displayNames.at(i));
#endif

    for (int i = 0; i < recentFiles.count(); i++)
    {
        QAction *action = recentFilesMenu->addAction(displayNames.at(i), this, SLOT(slotOpenFile()));
        action->setData(recentFiles.at(i));
        if (quitPending)
            action->setEnabled(false);
    }
}

#ifdef Q_OS_MAC
void QUDocumentWindowManager::addWindowMenuActions(QMenu *windowMenu, QUDocumentWindow *currentWindow)
{
    NSMenu *nsMenu = windowMenu->toNSMenu();
    NSInteger menuItemCount = [nsMenu numberOfItems];

    for (int i = 0; i < windows.count(); i++)
    {
        QUDocumentWindow *window = windows.at(i);
        QAction *action = windowMenu->addAction(window->windowTitle(), this, SLOT(slotShowWindow()));
        action->setData(i);
        action->setCheckable(true);
        if (window == currentWindow)
            action->setChecked(true);

        if (window->isMinimized())
            [[nsMenu itemAtIndex:menuItemCount + i] setState:NSMixedState];  // no diamond in Cocoa, just a hyphen
    }
}
#endif

void QUDocumentWindowManager::addToRecentFiles(const QString &fn)
{
    recentFiles.removeAll(fn);
    recentFiles.prepend(fn);

    while (recentFiles.size() > kMaxRecentFiles)
        recentFiles.removeLast();
}

void QUDocumentWindowManager::removeNonExistingRecentFiles()
{
    // Count down so indexes remain sane after deletions.
    for (int i = recentFiles.count() - 1; i >= 0; i--)
    {
        QFileInfo fileInfo(recentFiles.at(i));

        if (!fileInfo.exists())
            recentFiles.removeAt(i);
    }
}

// Only filenames are displayed as long as they're unique.  Identical filenames are suffixed with as much of the pathnames as necessary to distinguished them, starting the parent, then grandparent, etc.
QStringList QUDocumentWindowManager::recentFileDisplayNames()
{
    QStringList filenames;
    QList<QStringList> pathComponents;
    QList<QDir> directories;
    for (int i = 0; i < recentFiles.count(); i++)
    {
        QFileInfo fileInfo(recentFiles.at(i));

        filenames.append(fileInfo.fileName());
        pathComponents.append(QStringList());
        directories.append(fileInfo.dir());
    }

    bool changed;
    do
    {
        changed = false;
        for (int i = 0; i < recentFiles.count() - 1; i++)
        {
            bool found = false;
            for (int j = i + 1; j < recentFiles.count(); j++)
            {
                if (filenames.at(i) == filenames.at(j) && pathComponents.at(i) == pathComponents.at(j))
                {
                    found = true;
                    pathComponents[j].prepend(directories.at(j).dirName());
                    directories[j].cdUp();
                }
            }
            if (found)
            {
                pathComponents[i].prepend(directories.at(i).dirName());
                directories[i].cdUp();
                changed = true;
            }
        }
    }
    while (changed);

    QStringList result;

    for (int i = 0; i < recentFiles.count(); i++)
    {
        QString displayName = filenames.at(i);
        QStringList pathComponent = pathComponents.at(i);
        if (!pathComponent.isEmpty())
            displayName += tr(" — ") + pathComponent.join(tr(" ‣ "));
        result.append(displayName);
    }

    return result;
}

int QUDocumentWindowManager::countModifiedDocs()
{
    int modifiedDocCount = 0;

    for (int i = 0; i < windows.size(); i++)
        if (windows.at(i)->isWindowModified())
            modifiedDocCount++;

    return modifiedDocCount;
}

void QUDocumentWindowManager::forceCloseAllDocs()
{
    for (int i = 0; i < windows.size(); i++)
        windows.at(i)->definitelyClose();
}

void QUDocumentWindowManager::continueQuit()
{
    if (quitPending)
        closeNextDoc();
}

void QUDocumentWindowManager::cancelQuit()
{
#ifdef Q_OS_MAC
    if (quitPending)
        [[NSApp delegate] cancelTermination];
#endif
    quitPending = false;
    emit quitNotPending(!quitPending);
}

void QUDocumentWindowManager::newFile()
{

    // Find maximum sequence number among all untitled windows.  Add one gives new sequence number.
    int maxSeqNum = 0;
    for (int i = 0; i < windows.size(); i++)
        if (windows.at(i)->isUntitled)
        {
#ifdef Q_OS_MAC
            QRegExp rx("^untitled( (\\d+))?$");
#else
            QRegExp rx("^untitled( (\\d+))?(\\[\\*\\])?$");
#endif
            if (rx.indexIn(windows.at(i)->windowTitle()) != -1)
            {
                int seqNum = rx.cap(1).isEmpty() ? 1 : rx.cap(2).toInt();
                if (maxSeqNum < seqNum)
                    maxSeqNum = seqNum;
            }
        }

    QUDocumentWindow *w = QUDocumentWindow::createUntitled(maxSeqNum + 1);

#ifdef Q_OS_MAC
    cascade(w);
#endif

    windows.append(w);

    w->installEventFilter(this);

    w->show();

    onlyFirstUntitledDocOpened = noDocOpened;
    noDocOpened = false;
}

void QUDocumentWindowManager::open()
{
    QFileDialog openFileDialog(0);

    openFileDialog.setFileMode(QFileDialog::ExistingFile);
    openFileDialog.setNameFilter(tr("C files (*.c *.cc *.cpp *.h);;Text files (*.txt);;All Files (*)"));

    if (openFileDialog.exec())
        openFile(openFileDialog.selectedFiles()[0]);
}

void QUDocumentWindowManager::openFile(const QString &fn)
{
    for (int i = 0; i < windows.size(); i++)
    {
        if (windows.at(i)->currFile == fn)
        {
            windows.at(i)->activateWindow();

            return;
        }
    }

    if (onlyFirstUntitledDocOpened && !windows.isEmpty())
    {
        QUDocumentWindow *w = windows[0];
        if (w->loadFile(fn))
        {
            w->isUntitled = false;
            w->setCurrFile(fn);

            addToRecentFiles(fn);
        }
    }
    else
    {
        QUDocumentWindow *w = QUDocumentWindow::createFromFile(fn);

#ifdef Q_OS_MAC
        cascade(w);
#endif

        windows.append(w);

        w->installEventFilter(this);

        w->show();
    }

    onlyFirstUntitledDocOpened = false;
    noDocOpened = false;
}

#ifdef Q_OS_MAC
void QUDocumentWindowManager::bringAllToFront()
{
    ProcessSerialNumber PSN;

    if (GetCurrentProcess(&PSN) == noErr)
        SetFrontProcess(&PSN);
}
#endif

void QUDocumentWindowManager::about()
{
#ifdef Q_OS_MAC
    QMessageBox::about(0, tr("About QtDocBasedApp"), tr("<h2>QtDocBasedApp 1.0</h2><p>Copyright &copy; 2009 Andrew Choi."));
#else
    QMessageBox::about(qApp->activeWindow(), tr("About QtDocBasedApp"), tr("<h2>QtDocBasedApp 1.0</h2><p>Copyright &copy; 2009 Andrew Choi."));
#endif
}

void QUDocumentWindowManager::help()
{
#ifdef Q_OS_MAC
    QMessageBox::about(0, tr("QtDocBasedApp Help"), tr("Help!"));
#else
    QMessageBox::about(qApp->activeWindow(), tr("QtDocBasedApp Help"), tr("Help!"));
#endif
}

void QUDocumentWindowManager::closeDocumentsAndQuit()
{
    if (quitPending)
    {
        QApplication::beep();
        return;
    }

    int modifiedDocCount = countModifiedDocs();

    if (modifiedDocCount <= 1)
    {
        closeAllUnmodifiedDocsAndInitiateQuit();
    }
    else
    {
        QMessageBox quitMessageBox(0);

        quitMessageBox.setIcon(QMessageBox::Warning);
        quitMessageBox.setText(tr("You have %1 QtDocBasedApp documents with unsaved changes. Do you want to review these changes before quitting?").arg(modifiedDocCount));
        quitMessageBox.setInformativeText(tr("If you don’t review your documents, all your changes will be lost."));
        quitMessageBox.setDefaultButton(quitMessageBox.addButton(tr("Review Changes..."), QMessageBox::AcceptRole));
        quitMessageBox.addButton(tr("Cancel"), QMessageBox::RejectRole);
        quitMessageBox.addButton(tr("Discard Changes"), QMessageBox::DestructiveRole);

        quitMessageBox.exec();

        switch(quitMessageBox.buttonRole(quitMessageBox.clickedButton()))
        {
            case QMessageBox::AcceptRole:
                closeAllUnmodifiedDocsAndInitiateQuit();

                break;
            case QMessageBox::DestructiveRole:
                forceCloseAllDocs();

#ifdef Q_OS_MAC
                [[NSApp delegate] confirmPendingTermination];
#endif
                qApp->quit();

                break;
#ifdef Q_OS_MAC
            case QMessageBox::RejectRole:
                [[NSApp delegate] cancelTermination];
#endif
            default:
                break;
        }
     }
}

// Handle selection of a file in the recent files menu.
void QUDocumentWindowManager::slotOpenFile()
{
    if (QAction *action = qobject_cast<QAction *>(sender()))
    {
        QVariant v = action->data();
        if (v.canConvert<QString>())
            openFile(qvariant_cast<QString>(v));
    }
}

#ifdef Q_OS_MAC
// Handle selection of a document window in the Window menu.
void QUDocumentWindowManager::slotShowWindow()
{
    if (QAction *action = qobject_cast<QAction *>(sender()))
    {
        QVariant v = action->data();
        if (v.canConvert<int>())
        {
            int offset = qvariant_cast<int>(v);
            QUDocumentWindow *w = windows.at(offset);

            if (w->isMinimized())
                w->showNormal();
            w->activateWindow();
        }
    }
}
#endif

#ifdef Q_OS_MAC
void QUDocumentWindowManager::slotAboutToShowFileMenu()
{
    openRecentSubMenu->clear();

    addRecentFilesMenuActions(openRecentSubMenu);
}
#endif

// N.B. Documents must be close in this sequential manner (instead of just calling close() on them all at once)!  Otherwise a strange timing issue on Mac and Windows will prevent window modal dialogs to not appear correctly when we ask for confirmation for the modified windows.
void QUDocumentWindowManager::closeAllUnmodifiedDocsAndInitiateQuit()
{
    bool found = false;
    for (int i = 0; i < windows.size(); i++)
    {
        QUDocumentWindow *w = windows[i];
        if (!w->isWindowModified())
        {
            connect(w, SIGNAL(destroyed()), this, SLOT(closeAllUnmodifiedDocsAndInitiateQuit()));
            w->close();
            found = true;
            break;
        }
    }

    if (!found)
        initiateQuit();
}

void QUDocumentWindowManager::saveRecentFilesSettings()
{
    QSettings settings;
    settings.setValue("recentFiles", recentFiles);
}

void QUDocumentWindowManager::initiateQuit()
{
    quitPending = true;
    emit quitNotPending(!quitPending);

    closeNextDoc();
}

void QUDocumentWindowManager::closeNextDoc()
{
    if (!windows.isEmpty())
    {
        QUDocumentWindow *w = windows[0];

        w->showNormal();
        w->activateWindow();
        w->close();
    }
    else
    {
#ifdef Q_OS_MAC
        [[NSApp delegate] confirmPendingTermination];
#endif
        qApp->quit();
    }
}

#ifdef Q_OS_MAC
// A new window is offset 22 pixels to the right and below the active window.  If it goes beyond the screen, make some necessary adjustments.
void QUDocumentWindowManager::cascade(QUDocumentWindow *w)
{
    QRect r = QApplication::desktop()->availableGeometry();

    QPoint p = r.topLeft() + QPoint(6, 6);

    if (!windows.isEmpty())
        p = windows.at(0)->pos() + QPoint(22, 22);

    if (p.x() + w->width() > r.right())
        p.rx() = 0;
    if (p.y() + w->height() > r.bottom() - 22)  // account for height of window title bar
        p.ry() = r.top();

    w->move(p);
}
#endif
