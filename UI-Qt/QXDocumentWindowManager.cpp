#include <QApplication>
#include <QDesktopWidget>
#include <QDockWidget>
#include <QFileDialog>
#include <QMenu>
#include <QMenuBar>
#include <QMessageBox>
#include <QMetaType>
#include <QtDebug>
#include <QtGui>

#include "FontX/FXFace.h"

#include "QXApplication.h"
#include "QXConv.h"
#include "QXDockTitleBarWidget.h"
#include "QXDocumentWindow.h"
#include "QXDocumentWindowManager.h"
#include "QXFontCollectionDialog.h"
#include "QXMenuBar.h"
#include "QXOpenFontDialog.h"

QXDocumentWindowManager * QXDocumentWindowManager::instance_ = nullptr;

QXDocumentWindowManager::QXDocumentWindowManager() {
#ifdef Q_OS_MAC
    // Create default menu bar.
    new QXMenuBar();
#endif

    loadRecentFontSettings();
    connect(qApp, &QXApplication::aboutToQuit, this, &QXDocumentWindowManager::saveRecentFontsSettings);
#if 0
    connect(qApp, &QApplication::focusChanged, [](QWidget * oldWidget, QWidget * newWidget) {
        QWidget * dockWidget = newWidget;
        while (dockWidget && !qobject_cast<QDockWidget*>(dockWidget))
            dockWidget = dockWidget->parentWidget();
        
        if (!dockWidget) {
            QWidget * dockWidget = oldWidget;
            while (dockWidget && !qobject_cast<QDockWidget*>(dockWidget))
                dockWidget = dockWidget->parentWidget();
        }
        
        if (dockWidget) {
            QWidget * titleBarWidget = (qobject_cast<QDockWidget*>(dockWidget))->titleBarWidget();
            if (titleBarWidget)
                titleBarWidget->update();
        }
    });
#endif
}

QXDocumentWindowManager::~QXDocumentWindowManager() {
    delete openFontDialog_;
}

// Singleton
QXDocumentWindowManager *
QXDocumentWindowManager::instance() {
    if (instance_ == nullptr)
        instance_ = new QXDocumentWindowManager;
    return instance_;
}

void
QXDocumentWindowManager::addDocument(QXDocument * document) {
    if (documents_.indexOf(document) == -1)        
        documents_.append(document);
}

void
QXDocumentWindowManager::removeDocument(QXDocument * document) {
    for (int i = documents_.count() - 1; i >= 0; --i)
        if (documents_.at(i) == document)
            documents_.removeAt(i);

}

const QList<QPointer<QXDocument> > &
QXDocumentWindowManager::documents() const {
    return documents_;
}

QXDocument *
QXDocumentWindowManager::getDocument(const QXFontURI & fontURI) const {
    foreach (QPointer<QXDocument> document, documents_) {
        if (document->uri() == fontURI)
            return document;
    }
    return nullptr;
}
    
QXDocumentWindow *
QXDocumentWindowManager::getDocumentWindow(const QXDocument * document) const {
    foreach(QPointer<QXDocumentWindow> window, documentWindows_) {
        if (window->document() == document)
            return window;
    }
    return nullptr;
}

QXDocumentWindow *
QXDocumentWindowManager::createDocumentWindow(QXDocument * document) {
    QXDocumentWindow * window = new QXDocumentWindow(document, nullptr);

    connect(window,
            &QXDocumentWindow::aboutToClose,
            this,
            &QXDocumentWindowManager::onDocumentWindowAboutToClose);

    connect(window,
            &QXDocumentWindow::destroyed,
            this,
            &QXDocumentWindowManager::onDocumentWindowDestroyed);
    
    documentWindows_.append(window);
    return window;            
}

void
QXDocumentWindowManager::removeDocumentWindow(QXDocumentWindow * window)
{
    // remove from window list
    for (int i = documentWindows_.count() - 1; i >= 0; --i)
        if (documentWindows_.at(i) == window)
            documentWindows_.removeAt(i);
}

const QList<QXRecentFontItem> &
QXDocumentWindowManager::recentFonts() const {
    return recentFonts_;
}

void
QXDocumentWindowManager::aboutToShowWindowMenu(QMenu * menu) {
    QList<QAction *> actionsToRemove;
    for (QAction * action: menu->actions()) {
        QVariant data = action->data();
        if (data.canConvert<QXFontURI>())
            actionsToRemove.append(action);
    }

    for (QAction * action: actionsToRemove) {
        menu->removeAction(action);
    }

    foreach(QXDocumentWindow * window, documentWindows_) {
        QAction * action = new QAction(window->windowTitle(), menu);
        connect(action, &QAction::triggered, this, [window]() {
            window->show();
            window->raise();
            window->activateWindow();
        });
        action->setData(QVariant::fromValue<QXFontURI>(window->document()->uri()));

        QWidget * parent = menu;
        while ((parent = parent->parentWidget())) {
            if (parent == window) {
                action->setCheckable(true);
                action->setChecked(true);
                break;
            }
        }
        
        menu->addAction(action);
    }
}

void
QXDocumentWindowManager::aboutToShowRecentMenu(QMenu * recentMenu) {
    recentMenu->clear();
    foreach(QXRecentFontItem font, recentFonts_) {
        QAction * action = recentMenu->addAction(font.fullName);
        action->setData(QVariant::fromValue<QXFontURI>(font));
    }
}

void
QXDocumentWindowManager::doOpenFontDialog() {
    if (!openFontDialog_)
        openFontDialog_ = new QXOpenFontDialog(nullptr);
    if (QDialog::Accepted == openFontDialog_->exec()) {
        const QXFontURI fontURI = openFontDialog_->selectedFont();
        openFontURI(fontURI);
    }
}

void
QXDocumentWindowManager::autoOpenFontDialog() {
    if (documents().empty()) {
        bool hasWindow = false;
        for (QWidget * widget: qApp->allWidgets()) {
            QWidget * window = widget->window();
            if (window &&
                window->isWindow() &&
                window->isVisible()
                && !qobject_cast<QMenuBar*>(window)
                && !qobject_cast<QMenu*>(window))
            {
                hasWindow = true;
                break;
            }
        }
        if (!hasWindow)
            doOpenFontDialog();
    }
}

void
QXDocumentWindowManager::onDocumentWindowAboutToClose(QXDocumentWindow * window) {
    // remove from list
    removeDocumentWindow(window);
    removeDocument(window->document());
}

void
QXDocumentWindowManager::onDocumentWindowDestroyed(QObject * obj) {
#ifdef Q_OS_MAC
    // show Open Font dialog if no document open
    if (!quitRequested_ && documents_.empty())
        doOpenFontDialog();
#endif    
}

void
QXDocumentWindowManager::addToRecents(QXDocument * document) {
    QXRecentFontItem item;
    item.filePath  = document->uri().filePath;
    item.faceIndex = document->uri().faceIndex;
    item.fullName  = QXDocument::faceDisplayName(document->face()->attributes());

    // add or move to front
    int index = recentFonts_.indexOf(item);
    if (index != -1)
        recentFonts_.takeAt(index);
    recentFonts_.insert(0, item); 

    while (recentFonts_.size() > kMaxRecentFiles)
        recentFonts_.takeLast();
}

void
QXDocumentWindowManager::doOpenFontFromFile() {
    QFileDialog openFileDialog(nullptr);

    openFileDialog.setFileMode(QFileDialog::ExistingFile);
    openFileDialog.setNameFilter(tr("C files (*.c *.cc *.cpp *.h);;Text files (*.txt);;All Files (*)"));

    if (QDialog::Accepted == openFileDialog.exec())
        openFontFile(openFileDialog.selectedFiles()[0]);
}

void
QXDocumentWindowManager::openFontFile(const QString & filePath) {
    size_t faceCount = FXCountFaces(toStdString(filePath));
    if (faceCount == 1) {
        QXFontURI uri {filePath, 0};
        openFontURI(uri);
    }
    else if (faceCount > 1) {
        int index = QXFontCollectionDialog::selectFontIndex(filePath);
        if (index != -1) {
            QXFontURI uri {filePath, static_cast<size_t>(index)};
            openFontURI(uri);
        }
    }
    else {
        showOpenFontFileError(filePath);
    }
}

void
QXDocumentWindowManager::openFontURI(const QXFontURI & uri) {
    if (openFontDialog_ && openFontDialog_->isVisible())
        openFontDialog_->close();

    // check already open
    QXDocument * document = getDocument(uri);
    if (document) {
        QXDocumentWindow * window = getDocumentWindow(document);
        window->setWindowState((window->windowState() & ~Qt::WindowMinimized) | Qt::WindowActive);
        window->activateWindow();
        window->raise();
    }
    else {
        // or open new
        document = QXDocument::openFromURI(uri, this);
        if (document) {
            addDocument(document);
            addToRecents(document);
            QXDocumentWindow * window = createDocumentWindow(document);
            window->show();
        } else {
            showOpenFontFileError(uri.filePath);
        }
    }
}


#ifdef Q_OS_MAC
// Handle selection of a document window in the Window menu.
void
QXDocumentWindowManager::slotShowWindow() {
    if (QAction *action = qobject_cast<QAction *>(sender())) {
        QVariant v = action->data();
        if (v.canConvert<int>()) {
            int offset = qvariant_cast<int>(v);
            QXDocumentWindow *w = documentWindows_.at(offset);

            if (w->isMinimized())
                w->showNormal();
            w->activateWindow();
        }
    }
}

#endif

void
QXDocumentWindowManager::showOpenFontFileError(const QString & file) {
    QMessageBox::critical(nullptr,
                      tr("Error to open file"),
                      tr("The file %1 can't be open.").arg(file),
                      QMessageBox::Ok);
}

void
QXDocumentWindowManager::loadRecentFontSettings() {
    qRegisterMetaTypeStreamOperators<QXRecentFontItem>("QURecentFontItem");

    QSettings settings;
    QList<QVariant> variantList = settings.value("recentFonts").toList();
    foreach(QVariant v, variantList) {
        if (v.canConvert<QXRecentFontItem>())
            recentFonts_.append(v.value<QXRecentFontItem>());
    }
}

void
QXDocumentWindowManager::saveRecentFontsSettings() {
    qRegisterMetaTypeStreamOperators<QXRecentFontItem>("QURecentFontItem");

    QSettings settings;
    QList<QVariant> variantList;
    foreach(QXRecentFontItem uri, recentFonts_)
        variantList.append(QVariant::fromValue(uri));

    settings.setValue("recentFonts", variantList);
}

void
QXDocumentWindowManager::closeAllDocumentsAndQuit() {
    for (int i = 0; i < documentWindows_.size(); i++) {
        QXDocumentWindow * w = documentWindows_[i];
        w->close();
    }
    
    saveRecentFontsSettings();
    quitRequested_ = true;
    qApp->quit();
}
