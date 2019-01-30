#include <QtGui>
#include <QtDebug>
#include <QMenu>
#include <QMenuBar>
#include <QDesktopWidget>
#include <QApplication>
#include <QMessageBox>
#include <QFileDialog>
#include <QMetaType>

#ifdef Q_OS_MAC
#  import <Cocoa/Cocoa.h>
#  import "MacApplicationDelegate.h"
#endif

#include "QUConv.h"
#include "QUApplication.h"
#include "QUOpenFontDialog.h"
#include "QUDocumentWindow.h"
#include "QUDocumentWindowManager.h"
#include "QUMenuBar.h"

QUDocumentWindowManager * QUDocumentWindowManager::instance_ = nullptr;

QUDocumentWindowManager::QUDocumentWindowManager()
{
#ifdef Q_OS_MAC
    // Create default menu bar.
    QMenuBar * mb = new QUMenuBar();
    
#if 0
    QMenu *fileMenu = mb->addMenu(tr("&File"));
    fileMenu->addAction(tr("&Open..."),
                        this,
                        &QUDocumentWindowManager::doOpenFontDialog,
                        QKeySequence::Open);

    fileMenu->addAction(tr("Open &File..."),
                        this,
                        &QUDocumentWindowManager::doOpenFontFromFile,
                        QKeySequence(Qt::CTRL | Qt::SHIFT | Qt::Key_O));

    openRecentSubMenu = fileMenu->addMenu(tr("Open Recent"));
    connect(fileMenu, &QMenu::aboutToShow, this, &QUDocumentWindowManager::slotAboutToShowFileMenu);
    
    
    QMenu *helpMenu = mb->addMenu(tr("Help"));
    helpMenu->addAction(tr("&About"), quApp, &QUApplication::about);
#endif
#endif

    loadRecentFontSettings();
    connect(quApp, &QUApplication::aboutToQuit, this, &QUDocumentWindowManager::saveRecentFontsSettings);
}

// Singleton
QUDocumentWindowManager *
QUDocumentWindowManager::instance() {
    if (instance_ == nullptr)
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
            &QUDocumentWindow::aboutToClose,
            this,
            &QUDocumentWindowManager::onDocumentWindowAboutToClose);

    connect(window,
            &QUDocumentWindow::destroyed,
            this,
            &QUDocumentWindowManager::onDocumentWindowDestroyed);
    
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

const QList<QURecentFontItem> &
QUDocumentWindowManager::recentFonts() const {
    return recentFonts_;
}

void
QUDocumentWindowManager::aboutToShowWindowMenu(QMenu * menu) {
    return;
    menu->clear();
    foreach(QUDocumentWindow * window, documentWindows_) {
        QAction * action = new QAction(window->windowTitle(), menu);
        connect(action, &QAction::triggered, this, [window]() {
            window->show();
            window->raise();
            window->activateWindow();
        });
        menu->addAction(action);
    }
}

void
QUDocumentWindowManager::aboutToShowRecentMenu(QMenu * recentMenu) {
    recentMenu->clear();
    foreach(QURecentFontItem font, recentFonts_) {
        QAction * action = recentMenu->addAction(font.fullName);
        action->setData(QVariant::fromValue<QUFontURI>(font));
    }
}

void
QUDocumentWindowManager::doOpenFontDialog() {
    QUOpenFontDialog openDialog(nullptr);
    if (QDialog::Accepted == openDialog.exec()) {
        const QUFontURI fontURI = openDialog.selectedFont();
        openFont(fontURI);
    }
    return;
}

void
QUDocumentWindowManager::onDocumentWindowAboutToClose(QUDocumentWindow * window) {
    // remove from list
    removeDocumentWindow(window);
    removeDocument(window->document());
}

void
QUDocumentWindowManager::onDocumentWindowDestroyed(QObject * obj) {
#ifdef Q_OS_MAC
    // show Open Font dialog if no document open
    if (!quitRequested_ && documents_.empty())
        doOpenFontDialog();
#endif    
}

void
QUDocumentWindowManager::addToRecents(QUDocument * document) {
    QURecentFontItem item;
    item.filePath  = document->uri().filePath;
    item.faceIndex = document->uri().faceIndex;
    item.fullName  = QUDocument::faceGUIName(document->face()->attributes());

    int index = recentFonts_.indexOf(item);
    if (index != -1)
        recentFonts_.takeAt(index);
    recentFonts_.insert(0, item); // add or move to front
}

void
QUDocumentWindowManager::doOpenFontFromFile() {
    QFileDialog openFileDialog(nullptr);

    openFileDialog.setFileMode(QFileDialog::ExistingFile);
    openFileDialog.setNameFilter(tr("C files (*.c *.cc *.cpp *.h);;Text files (*.txt);;All Files (*)"));

    if (openFileDialog.exec())
        openFile(openFileDialog.selectedFiles()[0]);
}

void
QUDocumentWindowManager::openFile(const QString & filePath) {
    QUFontURI uri {filePath, 0};
    openFont(uri);
}

void
QUDocumentWindowManager::openFont(const QUFontURI & uri) {
    // check already open
    QUDocument * document = getDocument(uri);
    if (document) {
        QUDocumentWindow * window = getDocumentWindow(document);
        window->setWindowState((window->windowState() & ~Qt::WindowMinimized) | Qt::WindowActive);
        window->activateWindow();
        window->raise();
    }
    else {
        // or open new
        document = QUDocument::openFromURI(uri, this);
        if (document) {
            addDocument(document);
            addToRecents(document);
            QUDocumentWindow * window = createDocumentWindow(document);
            window->show();
        }
    }
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

void
QUDocumentWindowManager::loadRecentFontSettings() {
    qRegisterMetaTypeStreamOperators<QURecentFontItem>("QURecentFontItem");

    QSettings settings;
    QList<QVariant> variantList = settings.value("recentFonts").toList();
    foreach(QVariant v, variantList) {
        if (v.canConvert<QURecentFontItem>())
            recentFonts_.append(v.value<QURecentFontItem>());
    }
}

void
QUDocumentWindowManager::saveRecentFontsSettings() {
    qRegisterMetaTypeStreamOperators<QURecentFontItem>("QURecentFontItem");

    QSettings settings;
    QList<QVariant> variantList;
    foreach(QURecentFontItem uri, recentFonts_)
        variantList.append(QVariant::fromValue(uri));

    settings.setValue("recentFonts", variantList);
}

void
QUDocumentWindowManager::closeAllDocumentsAndQuit() {
    for (int i = 0; i < documentWindows_.size(); i++) {
        QUDocumentWindow * w = documentWindows_[i];
        w->close();
    }
    
    saveRecentFontsSettings();
    quitRequested_ = true;
    qApp->quit();
}
