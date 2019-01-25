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
#ifdef Q_OS_MAC
    // Create default menu bar.
    QMenuBar *mb = new QMenuBar;

    QMenu *fileMenu = mb->addMenu(tr("&File"));
    fileMenu->addAction(tr("&Open..."), this, &QUDocumentWindowManager::doOpenFontDialog, QKeySequence::Open);
    fileMenu->addAction(tr("Open &File..."), this, &QUDocumentWindowManager::slotOpenFile, QKeySequence(Qt::CTRL | Qt::SHIFT | Qt::Key_O));

    openRecentSubMenu = fileMenu->addMenu(tr("Open Recent"));
    connect(fileMenu, &QMenu::aboutToShow, this, &QUDocumentWindowManager::slotAboutToShowFileMenu);
    
    
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
    if (documents_.indexOf(document) == -1)        
        documents_.append(document);
}

void
QUDocumentWindowManager::removeDocument(QUDocument * document) {
    for (int i = documents_.count() - 1; i >= 0; --i)
        if (documents_.at(i) == document)
            documents_.removeAt(i);

}

const QList<QPointer<QUDocument> > &
QUDocumentWindowManager::documents() const {
    return documents_;
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

const QList<QUFontURI> &
QUDocumentWindowManager::recentFonts() const {
    return recentFonts_;
}

void
QUDocumentWindowManager::doOpenFontDialog()
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
                addToRecents(fontURI);
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

    // show Open Font dialog if no document open
    if (documents_.empty())
        doOpenFontDialog();
}

///////////////////////////////////////////////////////////////////////////////////////////

void
QUDocumentWindowManager::addToRecents(const QUFontURI & uri) {
    int index = recentFonts_.indexOf(uri);
    if (index != -1)
        recentFonts_.takeAt(index);
    recentFonts_.insert(0, uri); // add or move to front
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


}


void QUDocumentWindowManager::about()
{
    QMessageBox::about(qApp->activeWindow(), tr("About QtDocBasedApp"), tr("<h2>QtDocBasedApp 1.0</h2><p>Copyright &copy; 2009 Andrew Choi."));
}

void QUDocumentWindowManager::help()
{
    QMessageBox::about(qApp->activeWindow(), tr("FontViewer Help"), tr("Help meeeeeeee!"));
}

#ifdef Q_OS_MAC
// Handle selection of a document window in the Window menu.
void
QUDocumentWindowManager::slotShowWindow()
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

void
QUDocumentWindowManager::slotAboutToShowFileMenu() {
    
}

#endif

void QUDocumentWindowManager::saveRecentFilesSettings()
{
    QSettings settings;
//    settings.setValue("recentFiles", recentFonts_);
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
    qApp->quit();
}
