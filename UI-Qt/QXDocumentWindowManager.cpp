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
#include <QSplashScreen>

#include "FontX/FXFace.h"

#include "QXApplication.h"
#include "QXConv.h"
#include "QXDockTitleBarWidget.h"
#include "QXDocumentWindow.h"
#include "QXDocumentWindowManager.h"
#include "QXFontBrowser.h"
#include "QXFontCollectionDialog.h"
#include "QXMenuBar.h"

namespace {
    QString fileTypeFilterToString(QXDocumentWindowManager::FileTypeFilter filter) {
        QString string;
        switch (filter) {
        case QXDocumentWindowManager::FileTypeFilter::Font:
            string = QXDocumentWindowManager::tr("Font files (*.ttf *.otf *.ttc *.pfa *.pfb)");
            break;
        case QXDocumentWindowManager::FileTypeFilter::PDF:
            string = QXDocumentWindowManager::tr("PDF files (*.pdf)");
            break;
        default:
            string = QXDocumentWindowManager::tr("All Files (*)");
            break;
        }
        return string;
    }

    QString fileTypeFiltersFullString() {
        QXDocumentWindowManager::FileTypeFilter filters[] = {
            QXDocumentWindowManager::FileTypeFilter::Font,
            QXDocumentWindowManager::FileTypeFilter::PDF,
            QXDocumentWindowManager::FileTypeFilter::All
        };
        QStringList text;
        for (auto filter : filters)
            text << fileTypeFilterToString(filter);
        return text.join(";;");
    }
}

QXDocumentWindowManager * QXDocumentWindowManager::instance_ = nullptr;

QXDocumentWindowManager::QXDocumentWindowManager() {
#ifdef Q_OS_MAC
    // Create default menu bar.
    new QXMenuBar();
#endif
    recentFonts_ = QXPreferences::recentFonts();

    connect(qApp, &QXApplication::aboutToQuit, [this]() {
        appIsAboutToQuit_ = true;
        QXPreferences::setRecentFonts(recentFonts_);
    });
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
#if 0
    QXFontBrowser browser(nullptr);
    if (QDialog::Accepted == browser.exec()) {
        const QXFontURI fontURI = browser.selectedFont();
        openFontURI(fontURI);
    }
#else
    if (!openFontDialog_) {
        openFontDialog_ = new QXFontBrowser(nullptr);
#if !defined(Q_OS_MAC)
        // This piece of shit makes qApp quit
        connect(openFontDialog_, &QDialog::rejected, this, [this]() {
            if (documents_.empty()) {
                delete openFontDialog_;
                openFontDialog_ = nullptr;
                closeAllDocumentsAndQuit();
            }
        }, Qt::QueuedConnection);
#endif
    }
    if (openFontDialog_ && QDialog::Accepted == openFontDialog_->exec()) {
        const QXFontURI fontURI = openFontDialog_->selectedFont();
        openFontURI(fontURI);
    }
#endif
}

void
QXDocumentWindowManager::autoOpenFontDialog() {
    if (appIsAboutToQuit_)
        return;
    if (documents().empty()) {
        bool hasWindow = false;
        for (QWidget * widget: qApp->allWidgets()) {
            QWidget * window = widget->window();
            if (window &&
                window->isWindow() &&
                window->isVisible()
                && !qobject_cast<QMenuBar*>(window)
                && !qobject_cast<QMenu*>(window)
                && !qobject_cast<QSplashScreen*>(window))
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
    Q_UNUSED(obj);
#ifdef Q_OS_MAC
    // show Open Font dialog if no document open
    if (!appIsAboutToQuit_ && documents_.empty())
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

    //QXJumpListHelper::addRecentFontItem(item);
}

bool
QXDocumentWindowManager::doOpenFontFromFile(FileTypeFilter selectedTypeFilter) {
    QFileDialog openFileDialog(nullptr);

    openFileDialog.setFileMode(QFileDialog::ExistingFile);
    openFileDialog.setNameFilter(fileTypeFiltersFullString());
    openFileDialog.selectNameFilter(fileTypeFilterToString(selectedTypeFilter));

    if (QDialog::Accepted == openFileDialog.exec()) {
        return openFontFile(openFileDialog.selectedFiles()[0]);
    }
    return false;
}

bool
QXDocumentWindowManager::openFontFile(const QString & filePath) {
    auto initFace = FXFace::createFace(toStdString(filePath), 0);
    size_t faceCount = initFace? initFace->faceCount(): 0;
    if (faceCount == 1) {
        QXFontURI uri {filePath, 0};
        return openFontURI(uri, initFace);
    }
    else if (faceCount > 1) {
        int index = QXFontCollectionDialog::selectFontIndex(filePath, initFace);
        if (index != -1) {
            QXFontURI uri {filePath, static_cast<size_t>(index)};
            return openFontURI(uri, initFace);
        }
    }
    else {
        showOpenFontFileError(filePath);
    }
    return false;
}

bool
QXDocumentWindowManager::openFontURI(const QXFontURI & uri, FXPtr<FXFace> initFace) {
    if (openFontDialog_ && openFontDialog_->isVisible())
        openFontDialog_->close();

    // check already open
    QXDocument * document = getDocument(uri);
    if (document) {
        QXDocumentWindow * window = getDocumentWindow(document);
        window->setWindowState((window->windowState() & ~Qt::WindowMinimized) | Qt::WindowActive);
        window->activateWindow();
        window->raise();
        return true;
    }
    else {
        // or open new
        document = QXDocument::openFromURI(uri, initFace, this);
        if (document) {
            addDocument(document);
            addToRecents(document);
            QXDocumentWindow * window = createDocumentWindow(document);
            window->show();
            return true;
        } else {
            showOpenFontFileError(uri.filePath);
            return false;
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
QXDocumentWindowManager::closeAllDocumentsAndQuit() {
    for (int i = 0; i < documentWindows_.size(); i++) {
        QXDocumentWindow * w = documentWindows_[i];
        w->close();
    }
    
    QXPreferences::setRecentFonts(recentFonts_);
    delete openFontDialog_;
    openFontDialog_ = nullptr;
    appIsAboutToQuit_ = true;
    QTimer::singleShot(100, qApp, &QXApplication::quit);
}
