#include <QApplication>
#include <QDesktopWidget>
#include <QDockWidget>
#include <QFileDialog>
#include <QInputDialog>
#include <QMenu>
#include <QMenuBar>
#include <QMessageBox>
#include <QMetaType>
#include <QSplashScreen>
#include <QtDebug>
#include <QtGui>

#include "FontX/FXFace.h"

#include "QXApplication.h"
#include "QXConv.h"
#include "QXDockTitleBarWidget.h"
#include "QXDocumentWindow.h"
#include "QXDocumentWindowManager.h"
#include "QXFontListView.h"
#include "QXFontListWindow.h"
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
    delete fontListWindow_;
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

const QList<QXDocument *> &
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
    for (auto window: documentWindows()) {
        if (window->document() == document)
            return window;
    }
    return nullptr;
}

QXDocumentWindow *
QXDocumentWindowManager::createDocumentWindow(QXDocument * document) {
    QXDocumentWindow * window = new QXDocumentWindow(document, nullptr);
    addManagedWindow(window);
    return window;            
}

void
QXDocumentWindowManager::addManagedWindow(QWidget * window)
{
    connect(window, &QWidget::destroyed, [window, this]() {
        removeManagedWindow(window);
        if (auto itr = windowToDocumentMap_.find(window); itr != windowToDocumentMap_.end())
            removeDocument(itr.value());
#if defined(Q_OS_MACOS)
        if (!appIsAboutToQuit_ && documents_.empty())
            showFontListWindow();
#endif
    });
       
    managedWindows_.append(window);
    
    if (auto documentWindow = qobject_cast<QXDocumentWindow *>(window))
        windowToDocumentMap_[window] = documentWindow->document();
}

void
QXDocumentWindowManager::removeManagedWindow(QWidget * window)
{
    // remove from window list
    for (int i = managedWindows_.count() - 1; i >= 0; --i)
        if (managedWindows_.at(i) == window)
            managedWindows_.removeAt(i);
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
        if (data.canConvert<bool>())
            actionsToRemove.append(action);
    }

    for (QAction * action: actionsToRemove) {
        menu->removeAction(action);
    }
    
    foreach(QWidget * window, managedWindows_) {
        QAction * action = new QAction(window->windowTitle(), menu);
        connect(action, &QAction::triggered, this, [window]() {
            qApp->bringWindowToFront(window);
        });
        
        action->setData(QVariant(true));
        action->setCheckable(true);
        action->setChecked(window->isActiveWindow());
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

bool
QXDocumentWindowManager::handleDropEvent(QDropEvent * event) {
    bool ok = false;
    for (const QUrl & url: event->mimeData()->urls()) {
        QString filePath = url.toLocalFile();
        ok |= QXDocumentWindowManager::instance()->openFontFile(filePath);
    }
    return ok;
}

void
QXDocumentWindowManager::showFontListWindow() {
    if (!fontListWindow_) {
        fontListWindow_ = new QXFontListWindow(nullptr);
        connect(fontListWindow_, &QXFontListWindow::fontSelected, this, [this]() {
            const QXFontURI fontURI = fontListWindow_->selectedFont();
            openFontURI(fontURI);
        });
    }

    qApp->bringWindowToFront(fontListWindow_);
    fontListWindow_->searchLineEdit()->setFocus();
}

void
QXDocumentWindowManager::autoShowFontListWindow() {
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
            showFontListWindow();
    }
}

void
QXDocumentWindowManager::addToRecents(QXDocument * document) {
    QXRecentFontItem item;
    item.filePath  = document->uri().filePath;
    item.faceIndex = document->uri().faceIndex;
    item.fullName  = QXDocument::faceDisplayName(document->face()->attributes(), FXFaceLanguages::en);

    // add or move to front
    int index = recentFonts_.indexOf(item);
    if (index != -1)
        recentFonts_.takeAt(index);
    recentFonts_.insert(0, item); 

    while (recentFonts_.size() > kMaxRecentFiles)
        recentFonts_.takeLast();
}

bool
QXDocumentWindowManager::doNativeOpenFileDialog(FileTypeFilter selectedTypeFilter) {
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
QXDocumentWindowManager::doQuickOpenFontDialog() {
    QInputDialog dialog(nullptr);
    dialog.setInputMode(QInputDialog::TextInput);
    dialog.setWindowTitle(tr("Open Font File"));
    dialog.setLabelText(tr("Please input the font path:"));
    if (dialog.exec() == QDialog::Accepted) {
        if (auto text = dialog.textValue(); !text.isEmpty())
            return openFontFile(text);
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
    if (fontListWindow_ && fontListWindow_->isVisible())
        fontListWindow_->close();

    // check already open
    QXDocument * document = getDocument(uri);
    if (document) {
        qApp->bringWindowToFront(getDocumentWindow(document));
        return true;
    }
    else {
        // or open new
        document = QXDocument::openFromURI(uri, initFace, this);
        if (document) {
            addDocument(document);
            addToRecents(document);
            QXDocumentWindow * window = createDocumentWindow(document);
            if (auto activeWindow = activeDocumentWindow()) {
                window->show();
                window->move(activeWindow->pos() + QPoint(50, 50));
            }
            else {
                window->show();
            }
            return true;
        } else {
            showOpenFontFileError(uri.filePath);
            return false;
        }
    }
}


void
QXDocumentWindowManager::showOpenFontFileError(const QString & file) {
    QMessageBox::critical(nullptr,
                      tr("Error to open file"),
                      tr("The file %1 can't be open.").arg(file),
                      QMessageBox::Ok);
}

QXDocumentWindow *
QXDocumentWindowManager::activeDocumentWindow() const {
    for (auto window : documentWindows()) {
        if (window->isActiveWindow())
            return window;
    }
    return nullptr;
}
    

void
QXDocumentWindowManager::closeAllDocumentsAndQuit() {
    for (int i = 0; i < managedWindows_.size(); i++)
        managedWindows_[i]->close();
    
    QXPreferences::setRecentFonts(recentFonts_);
    delete fontListWindow_;
    fontListWindow_ = nullptr;
    appIsAboutToQuit_ = true;
    QTimer::singleShot(100, qApp, &QXApplication::quit);
}

QList<QXDocumentWindow *>
QXDocumentWindowManager::documentWindows() const {
    QList<QXDocumentWindow *> windows;
    for (auto widget: managedWindows_) {
        if (auto window = qobject_cast<QXDocumentWindow*>(widget))
            windows.append(window);
    }
    return windows;
}
