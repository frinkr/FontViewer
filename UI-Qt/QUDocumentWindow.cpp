/*
  Permission for the use of this code is granted only for research, educational, and non-commercial purposes.

  Redistribution of this code or its parts in source, binary, and any other form without permission, with or without modification, is prohibited.  Modifications include, but are not limited to, translation to other programming languages and reuse of tables, constant definitions, and API's defined in it.

  Andrew Choi is not liable for any losses or damages caused by the use of this code.

  Copyright 2009 Andrew Choi.
*/

#include <QtGui>
#include <QtDebug>
#include <QPlainTextEdit>
#include <QVBoxLayout>
#include <QMenu>
#include <QMenuBar>
#include <QFileDialog>
#include <QMessageBox>
#include <QAbstractButton>
#include "qudocumentwindowmanager.h"

#include "qudocumentwindow.h"

QUDocumentWindow::QUDocumentWindow(QWidget *parent) : QMainWindow(parent)
{
    setAttribute(Qt::WA_DeleteOnClose);

    QWidget *centralWidget = new QWidget(this);

    textEdit = new QPlainTextEdit;

    QVBoxLayout *layout = new QVBoxLayout;
    layout->setSpacing(0);
    layout->setMargin(0);
    layout->addWidget(textEdit);
    centralWidget->setLayout(layout);
    setCentralWidget(centralWidget);

    resize(400, 300);

    createActions();
    createMenus();

    shouldDefinitelyClose = false;
    shouldCloseAfterSaveAs = false;
}

QUDocumentWindow *QUDocumentWindow::createUntitled(int seqNum)
{
    QUDocumentWindow *w = new QUDocumentWindow;

    w->isUntitled = true;
    if (seqNum > 1)
        w->setCurrFile(tr("untitled %1").arg(seqNum));
    else
        w->setCurrFile(tr("untitled"));

    // Qt requires window title to be set and contain a '[*]' placeholder before setWindowModified is called.
    connect(w->textEdit->document(), SIGNAL(modificationChanged(bool)), w, SLOT(setWindowModified(bool)));

    return w;
}

QUDocumentWindow *QUDocumentWindow::createFromFile(const QString &fn)
{
    QUDocumentWindow *w = new QUDocumentWindow;

    if (w->loadFile(fn))
    {
        w->isUntitled = false;
        w->setCurrFile(fn);

        // See comment about '[*]' placeholder in createUntitled().
        connect(w->textEdit->document(), SIGNAL(modificationChanged(bool)), w, SLOT(setWindowModified(bool)));

        QUDocumentWindowManager::instance()->addToRecentFiles(fn);

        return w;
    }
    else
        return 0;
}

// This closes the document window without saving it whether it has been modified or not.
void QUDocumentWindow::definitelyClose()
{
    shouldDefinitelyClose = true;
    close();
}

void QUDocumentWindow::save()
{
    if (isUntitled)
        saveAs();
    else
        saveFile(currFile);
}

void QUDocumentWindow::saveAs()
{
    saveAsFileDialog = new QFileDialog(this, Qt::Dialog);  // lasts beyond invocation of this function
    saveAsFileDialog->setAttribute(Qt::WA_DeleteOnClose);

    saveAsFileDialog->setAcceptMode(QFileDialog::AcceptSave);
    saveAsFileDialog->setDefaultSuffix("txt");
    saveAsFileDialog->selectFile(currFile);

    connect(saveAsFileDialog, SIGNAL(finished(int)), this, SLOT(finishedSaveAs(int)));

    saveAsFileDialog->setWindowModality(Qt::WindowModal);
    saveAsFileDialog->show();
}

// We use the finished(int) signal for the save dialogs because we want to process every possible dismissal of the window modal dialog, including when it is dismissed when the close button in its window bar being clicked (on Linux).
void QUDocumentWindow::finishedSaveAs(int result)
{
    bool saveSucceeded = false;

    if (result == QDialog::Accepted)
    {
        QStringList files = saveAsFileDialog->selectedFiles();

        if (files.count() == 1)
        {
            saveSucceeded = saveFile(files[0]);
            // Document window is closed after a successful save only if saveAs() was invoked from saveAndClose().
            if (saveSucceeded && shouldCloseAfterSaveAs)
                definitelyClose();
        }
    }

    shouldCloseAfterSaveAs = false;  // reset for future calls to saveAs()

    if (saveSucceeded)
        QUDocumentWindowManager::instance()->continueQuit();
    else
        QUDocumentWindowManager::instance()->cancelQuit();
}

void QUDocumentWindow::finishedMaybeSave(int)
{
    QAbstractButton *button = saveMessageBox->clickedButton();

    if (saveMessageBox->buttonRole(button) == QMessageBox::AcceptRole)
        saveAndClose();
    else if (saveMessageBox->buttonRole(button) == QMessageBox::DestructiveRole)
    {
        definitelyClose();

        QUDocumentWindowManager::instance()->continueQuit();
    }
    else if (saveMessageBox->buttonRole(button) == QMessageBox::RejectRole)
        QUDocumentWindowManager::instance()->cancelQuit();
}

void QUDocumentWindow::minimize()
{
    if (isMinimized())
        showNormal();
    else
        showMinimized();
}

void QUDocumentWindow::zoom()
{
    if (isMaximized())
        showNormal();
    else
        showMaximized();
}

void QUDocumentWindow::slotAboutToShowFileMenu()
{
#ifdef Q_OS_MAC
    openRecentSubMenu->clear();

    QUDocumentWindowManager::instance()->addRecentFilesMenuActions(openRecentSubMenu);
#else
    fileMenu->clear();

    fileMenu->addAction(newAction);
    fileMenu->addAction(openAction);
    fileMenu->addSeparator();
    fileMenu->addAction(closeAction);
    fileMenu->addAction(saveAction);
    fileMenu->addAction(saveAsAction);
    fileMenu->addSeparator();
    QUDocumentWindowManager::instance()->addRecentFilesMenuActions(fileMenu);
    fileMenu->addSeparator();
    fileMenu->addAction(quitAction);
#endif
}

void QUDocumentWindow::slotAboutToShowEditMenu()
{
    pasteAction->setEnabled(textEdit->canPaste());
}

#ifdef Q_OS_MAC
void QUDocumentWindow::slotAboutToShowWindowMenu()
{
    windowMenu->clear();
    windowMenu->addAction(minimizeAction);
    windowMenu->addAction(zoomAction);
    windowMenu->addSeparator();
    windowMenu->addAction(bringAllToFrontAction);

    windowMenu->addSeparator();

    QUDocumentWindowManager::instance()->addWindowMenuActions(windowMenu, this);
}
#endif

void QUDocumentWindow::setCurrFile(const QString &fn)
{
    currFile = fn;

#ifdef Q_OS_MAC
    setWindowTitle(QFileInfo(fn).fileName());
#else
    setWindowTitle(QFileInfo(fn).fileName() + tr("[*]"));
#endif
}

bool QUDocumentWindow::loadFile(const QString &fn)
{
    if (!QFile::exists(fn))
        return false;

    QFile file(fn);
    if (!file.open(QFile::ReadOnly))
        return false;

    textEdit->setPlainText(QTextStream(&file).readAll());

    return true;
}

bool QUDocumentWindow::saveFile(const QString &fn)
{
    QFile file(fn);
    if (!file.open(QFile::WriteOnly | QFile::Text))
    {
        QMessageBox::warning(this, tr("QtDocBasedApp"), tr("Failed to write file %1:\n%2.").arg(fn).arg(file.errorString()));

        return false;
    }

    QTextStream(&file) << textEdit->toPlainText();

    isUntitled = false;
    setCurrFile(fn);

    textEdit->document()->setModified(false);

    QUDocumentWindowManager::instance()->addToRecentFiles(fn);

    return true;
}

void QUDocumentWindow::closeEvent(QCloseEvent *e)
{
    if (shouldDefinitelyClose || !isWindowModified())
    {
        // This window is removed from the "windows" list in DocumentWindowManager *before* this window sends the destroyed() event.  The event handler will then pick another window in that list to close.
        QUDocumentWindowManager::instance()->removeDocumentWindow(this);
#ifdef Q_OS_MAC
        // It seems necessary on Qt/Cocoa to emit this signal ourselves.
        emit destroyed();
#endif
        e->accept();
    }
    else
    {
        maybeSave();
        e->ignore();
    }
}

void QUDocumentWindow::saveAndClose()
{
    if (isUntitled)
    {
        // If shouldCloseAfterSaveAs is true, saveAs() closes the document window after a successful save.
        shouldCloseAfterSaveAs = true;
        saveAs();
    }
    else
    {
        // Again, document window is closed only after a successful save.  E.g., if the users tries to save to a read-only volume, the save operation will fail and the document window remains open.
        if (saveFile(currFile))
        {
            definitelyClose();
            QUDocumentWindowManager::instance()->continueQuit();
        }
    }
}

void QUDocumentWindow::maybeSave()
{
    if (isMinimized())
        showNormal();

    activateWindow();

    saveMessageBox = new QMessageBox(this);
    saveMessageBox->setAttribute(Qt::WA_DeleteOnClose);  // pass ownership to window system

    saveMessageBox->setIcon(QMessageBox::Warning);
    saveMessageBox->setText(tr("Do you want to save the changes you made in the document “%1”?").arg(currFile));
    saveMessageBox->setInformativeText(tr("Your changes will be lost if you don’t save them."));
    saveMessageBox->setDefaultButton(saveMessageBox->addButton(isUntitled ? tr("Save...") : tr("Save"), QMessageBox::AcceptRole));
    saveMessageBox->addButton(tr("Cancel"), QMessageBox::RejectRole);
    saveMessageBox->addButton(tr("Don’t Save"), QMessageBox::DestructiveRole);

    connect(saveMessageBox, SIGNAL(finished(int)), this, SLOT(finishedMaybeSave(int)));

    saveMessageBox->setWindowModality(Qt::WindowModal);
    saveMessageBox->show();
}

void QUDocumentWindow::createActions()
{
    newAction = new QAction(tr("&New"), this);
    newAction->setShortcut(tr("Ctrl+N"));
    newAction->setStatusTip(tr("Create a new text document"));
    connect(newAction, SIGNAL(triggered()), QUDocumentWindowManager::instance(), SLOT(newFile()));
    connect(QUDocumentWindowManager::instance(), SIGNAL(quitNotPending(bool)), newAction, SLOT(setEnabled(bool)));

    openAction = new QAction(tr("&Open..."), this);
    openAction->setShortcut(tr("Ctrl+O"));
    openAction->setStatusTip(tr("Open a text document"));
    connect(openAction, SIGNAL(triggered()), QUDocumentWindowManager::instance(), SLOT(open()));
    connect(QUDocumentWindowManager::instance(), SIGNAL(quitNotPending(bool)), openAction, SLOT(setEnabled(bool)));

    closeAction = new QAction(tr("&Close"), this);
    closeAction->setShortcut(tr("Ctrl+W"));
    closeAction->setStatusTip(tr("Close this document"));
    connect(closeAction, SIGNAL(triggered()), this, SLOT(close()));
    connect(QUDocumentWindowManager::instance(), SIGNAL(quitNotPending(bool)), closeAction, SLOT(setEnabled(bool)));

    saveAction = new QAction(tr("&Save"), this);
    saveAction->setShortcut(tr("Ctrl+S"));
    saveAction->setStatusTip(tr("Save this document"));
    connect(saveAction, SIGNAL(triggered()), this, SLOT(save()));
    connect(QUDocumentWindowManager::instance(), SIGNAL(quitNotPending(bool)), saveAction, SLOT(setEnabled(bool)));

    saveAsAction = new QAction(tr("&Save As..."), this);
    saveAsAction->setShortcut(tr("Ctrl+Shift+S"));
    saveAsAction->setStatusTip(tr("Save this document with filename"));
    connect(saveAsAction, SIGNAL(triggered()), this, SLOT(saveAs()));
    connect(QUDocumentWindowManager::instance(), SIGNAL(quitNotPending(bool)), saveAsAction, SLOT(setEnabled(bool)));

    quitAction = new QAction(tr("&Quit..."), this);
    quitAction->setShortcut(tr("Ctrl+Q"));
    quitAction->setStatusTip(tr("Quit QtDocBasedApp"));
    connect(quitAction, SIGNAL(triggered()), QUDocumentWindowManager::instance(), SLOT(closeDocumentsAndQuit()));

    undoAction = new QAction(tr("&Undo"), this);
    undoAction->setShortcut(tr("Ctrl+Z"));
    undoAction->setEnabled(false);
    connect(undoAction, SIGNAL(triggered()), textEdit, SLOT(undo()));
    connect(textEdit, SIGNAL(undoAvailable(bool)), undoAction, SLOT(setEnabled(bool)));

    redoAction = new QAction(tr("&Redo"), this);
    redoAction->setShortcut(tr("Ctrl+Shift+Z"));
    redoAction->setEnabled(false);
    connect(redoAction, SIGNAL(triggered()), textEdit, SLOT(redo()));
    connect(textEdit, SIGNAL(redoAvailable(bool)), redoAction, SLOT(setEnabled(bool)));

    cutAction = new QAction(tr("Cu&t"), this);
    cutAction->setShortcut(tr("Ctrl+X"));
    cutAction->setEnabled(false);
    connect(cutAction, SIGNAL(triggered()), textEdit, SLOT(cut()));
    connect(textEdit, SIGNAL(copyAvailable(bool)), cutAction, SLOT(setEnabled(bool)));

    copyAction = new QAction(tr("&Copy"), this);
    copyAction->setShortcut(tr("Ctrl+C"));
    copyAction->setEnabled(false);
    connect(copyAction, SIGNAL(triggered()), textEdit, SLOT(copy()));
    connect(textEdit, SIGNAL(copyAvailable(bool)), copyAction, SLOT(setEnabled(bool)));

    pasteAction = new QAction(tr("&Paste"), this);
    pasteAction->setShortcut(tr("Ctrl+V"));
    connect(pasteAction, SIGNAL(triggered()), textEdit, SLOT(paste()));

    deleteAction = new QAction(tr("&Delete"), this);
    connect(deleteAction, SIGNAL(triggered()), textEdit, SLOT(clear()));
    deleteAction->setEnabled(false);
    connect(textEdit, SIGNAL(copyAvailable(bool)), deleteAction, SLOT(setEnabled(bool)));

    selectAllAction = new QAction(tr("Select &All"), this);
    selectAllAction->setShortcut(tr("Ctrl+A"));
    connect(selectAllAction, SIGNAL(triggered()), textEdit, SLOT(selectAll()));

#ifdef Q_OS_MAC
    minimizeAction = new QAction(tr("&Minimize"), this);
    minimizeAction->setShortcut(tr("Ctrl+M"));
    connect(minimizeAction, SIGNAL(triggered()), this, SLOT(minimize()));

    zoomAction = new QAction(tr("&Zoom"), this);
    connect(zoomAction, SIGNAL(triggered()), this, SLOT(zoom()));

    bringAllToFrontAction = new QAction(tr("Bring &All To Front"), this);
    connect(bringAllToFrontAction, SIGNAL(triggered()), QUDocumentWindowManager::instance(), SLOT(bringAllToFront()));
#endif

    aboutAction = new QAction(tr("&About QtDocBasedApp"), this);
    connect(aboutAction, SIGNAL(triggered()), QUDocumentWindowManager::instance(), SLOT(about()));

    helpAction = new QAction(tr("QtDocBasedApp &Help"), this);
    connect(helpAction, SIGNAL(triggered()), QUDocumentWindowManager::instance(), SLOT(help()));
}

void QUDocumentWindow::createMenus()
{
    fileMenu = menuBar()->addMenu(tr("&File"));
#ifdef Q_OS_MAC
    fileMenu->addAction(newAction);
    fileMenu->addAction(openAction);

    openRecentSubMenu = fileMenu->addMenu(tr("Open &Recent"));
    connect(QUDocumentWindowManager::instance(), SIGNAL(quitNotPending(bool)), openRecentSubMenu, SLOT(setEnabled(bool)));

    fileMenu->addSeparator();
    fileMenu->addAction(closeAction);
    fileMenu->addAction(saveAction);
    fileMenu->addAction(saveAsAction);

    fileMenu->addAction(quitAction);
#endif
    connect(fileMenu, SIGNAL(aboutToShow()), this, SLOT(slotAboutToShowFileMenu()));
    slotAboutToShowFileMenu();

    editMenu = menuBar()->addMenu(tr("&Edit"));
    editMenu->addAction(undoAction);
    editMenu->addAction(redoAction);
    editMenu->addSeparator();
    editMenu->addAction(cutAction);
    editMenu->addAction(copyAction);
    editMenu->addAction(pasteAction);
    editMenu->addAction(deleteAction);
    editMenu->addAction(selectAllAction);
    connect(editMenu, SIGNAL(aboutToShow()), this, SLOT(slotAboutToShowEditMenu()));
    slotAboutToShowEditMenu();

#ifdef Q_OS_MAC
    windowMenu = menuBar()->addMenu(tr("&Window"));
    connect(windowMenu, SIGNAL(aboutToShow()), this, SLOT(slotAboutToShowWindowMenu()));
    slotAboutToShowWindowMenu();
#endif

    helpMenu = menuBar()->addMenu(tr("&Help"));
    helpMenu->addAction(aboutAction);
    helpMenu->addAction(helpAction);
}
