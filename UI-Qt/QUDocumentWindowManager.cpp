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
#  import "MacApplicationDelegate.h"
#endif

#include "QUOpenFontDialog.h"
#include "QUDocumentWindow.h"
#include "QUDocumentWindowManager.h"

QUDocumentWindowManager * QUDocumentWindowManager::instance_ = nullptr;

QUDocumentWindowManager::QUDocumentWindowManager()
{
    QSettings settings;
    recentFiles = settings.value("recentFiles").toStringList();

#ifdef Q_OS_MAC
    // Create default menu bar.
    QMenuBar *mb = new QMenuBar;

    QMenu *fileMenu = mb->addMenu(tr("&File"));
    fileMenu->addAction(tr("&Open..."), this, &QUDocumentWindowManager::slotOpenFont, QKeySequence::Open);
    fileMenu->addAction(tr("Open &File..."), this, &QUDocumentWindowManager::slotOpenFile, QKeySequence(Qt::CTRL | Qt::SHIFT | Qt::Key_O));

    openRecentSubMenu = fileMenu->addMenu(tr("Open Recent"));
    connect(fileMenu, &QMenu::aboutToShow, this, &QUDocumentWindowManager::slotAboutToShowFileMenu);
    slotAboutToShowFileMenu();
    
    QMenu *helpMenu = mb->addMenu(tr("Help"));
    helpMenu->addAction(tr("&About"), this, &QUDocumentWindowManager::about);
    helpMenu->addAction(tr("About &Qt"), qApp, &QApplication::aboutQt);
    
    helpMenu->addAction(tr("FontViewer &Help"), this, &QUDocumentWindowManager::help);
#endif

    connect(qApp, SIGNAL(aboutToQuit()), this, SLOT(saveRecentFilesSettings()));
}

// Singleton
QUDocumentWindowManager *
QUDocumentWindowManager::instance() {
    if (instance_ == 0)
        instance_ = new QUDocumentWindowManager;
    return instance_;
}

void
QUDocumentWindowManager::addDocument(QUDocument * document) {
    documents_.append(document);
}

void
QUDocumentWindowManager::removeDocument(QUDocument * document) {
    for (int i = documents_.count() - 1; i >= 0; --i)
        if (documents_.at(i) == document)
            documents_.removeAt(i);

}

QUDocument *
QUDocumentWindowManager::getDocument(const QUFontURI & fontURI) const {
    foreach (QPointer<QUDocument> document, documents_) {
        if (document->uri() == fontURI)
            return document;
    }
    return nullptr;
}
    
QUDocumentWindow *
QUDocumentWindowManager::getDocumentWindow(const QUDocument * document) const {
    foreach(QPointer<QUDocumentWindow> window, documentWindows_) {
        if (window->document() == document)
            return window;
    }
    return nullptr;
}

QUDocumentWindow *
QUDocumentWindowManager::createDocumentWindow(QUDocument * document) {
    QUDocumentWindow * window = new QUDocumentWindow(document, nullptr);

    connect(window,
            &QUDocumentWindow::destroyed,
            this,
            &QUDocumentWindowManager::slotDocumentWindowDestroyed);
    
    documentWindows_.append(window);
    return window;            
}

void
QUDocumentWindowManager::removeDocumentWindow(QUDocumentWindow * window)
{
    // remove from window list
    for (int i = documentWindows_.count() - 1; i >= 0; --i)
        if (documentWindows_.at(i) == window)
            documentWindows_.removeAt(i);
}

void
QUDocumentWindowManager::slotOpenFont()
{
    QUOpenFontDialog openDialog(0);
    if (QDialog::Accepted == openDialog.exec()) {
        const QUFontURI fontURI = openDialog.selectedFont();

        // check already open
        QUDocument * document = getDocument(fontURI);
        if (document) {
            QUDocumentWindow * window = getDocumentWindow(document);
            window->setWindowState((window->windowState() & ~Qt::WindowMinimized) | Qt::WindowActive);
            window->activateWindow();
            window->raise();
        }
        else {
            // or open new
            document = QUDocument::openFromURI(fontURI, this);
            if (document) {
                addDocument(document);
                QUDocumentWindow * window = createDocumentWindow(document);
                window->show();
            }
        }
    }
    return;
}

void
QUDocumentWindowManager::slotDocumentWindowDestroyed(QObject * obj) {
    // remove from list
    QUDocumentWindow * window = (QUDocumentWindow*)obj;
    removeDocumentWindow(window);
    removeDocument(window->document());
}

///////////////////////////////////////////////////////////////////////////////////////////
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
        QAction *action = recentFilesMenu->addAction(displayNames.at(i), this, SLOT(slotOpenRecentFile()));
        action->setData(recentFiles.at(i));
    }
}

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

void QUDocumentWindowManager::slotOpenFile()
{
    QFileDialog openFileDialog(0);

    openFileDialog.setFileMode(QFileDialog::ExistingFile);
    openFileDialog.setNameFilter(tr("C files (*.c *.cc *.cpp *.h);;Text files (*.txt);;All Files (*)"));

    if (openFileDialog.exec())
        openFile(openFileDialog.selectedFiles()[0]);
}

void QUDocumentWindowManager::openFile(const QString &fn)
{
    for (int i = 0; i < documentWindows_.size(); i++)
    {
        if (documentWindows_.at(i)->currFile == fn)
        {
            documentWindows_.at(i)->activateWindow();

            return;
        }
    }
#if 0
    QUDocumentWindow *w = QUDocumentWindow::createFromFile(fn);

#ifdef Q_OS_MAC
    cascade(w);
#endif
    w->installEventFilter(this);
        
    documentWindows_.append(w);
    w->show();
#endif
}


void QUDocumentWindowManager::about()
{
    QMessageBox::about(qApp->activeWindow(), tr("About QtDocBasedApp"), tr("<h2>QtDocBasedApp 1.0</h2><p>Copyright &copy; 2009 Andrew Choi."));
}

void QUDocumentWindowManager::help()
{
    QMessageBox::about(qApp->activeWindow(), tr("FontViewer Help"), tr("Help meeeeeeee!"));
}

// Handle selection of a file in the recent files menu.
void QUDocumentWindowManager::slotOpenRecentFile()
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
            QUDocumentWindow *w = documentWindows_.at(offset);

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


void QUDocumentWindowManager::saveRecentFilesSettings()
{
    QSettings settings;
    settings.setValue("recentFiles", recentFiles);
}

void QUDocumentWindowManager::closeAllDocumentsAndQuit()
{
    bool found = false;
    for (int i = 0; i < documentWindows_.size(); i++)
    {
        QUDocumentWindow *w = documentWindows_[i];
        if (!w->isWindowModified())
        {
            connect(w, SIGNAL(destroyed()), this, SLOT(closeAllUnmodifiedDocsAndInitiateQuit()));
            w->close();
            found = true;
            break;
        }
    }
    
    saveRecentFilesSettings();
}
