#pragma once

#include <QList>
#include <QObject>
#include <QPointer>
#include <QStringList>

#include "QXDocument.h"
#include "QXPreferences.h"

class QXDocumentWindow;
class QXOpenFontDialog;
class QXFontListWindow;
class QDropEvent;
class QMenu;
class QString;

class QXDocumentWindowManager : public QObject
{
    Q_OBJECT

public:
    enum class FileTypeFilter {
        Font   = 0x01,
        PDF    = 0x02,
        All    = 0xFF,
    };
public:
    QXDocumentWindowManager();
    ~QXDocumentWindowManager();

    static QXDocumentWindowManager *
    instance();

    void
    addDocument(QXDocument * document);

    void
    removeDocument(QXDocument * document);

    const QList<QXDocument *> &
    documents() const;

    QXDocument *
    getDocument(const QXFontURI & fontURI) const;
    
    QXDocumentWindow *
    getDocumentWindow(const QXDocument * document) const;

    QXDocumentWindow *
    createDocumentWindow(QXDocument * document);

    void
    addManagedWindow(QWidget * window);
    
    void
    removeManagedWindow(QWidget *w);
    
    const QList<QXRecentFontItem> &
    recentFonts() const;

    void
    aboutToShowWindowMenu(QMenu * windowMenu);

    void
    reloadRecentMenu(QMenu * recentMenu, bool includeIcon = false);

    bool
    handleDropEvent(QDropEvent * event);

public slots:
    void
    showFontListWindow();

    void
    autoShowFontListWindow();

    void
    closeAllDocuments();
    
    void
    closeAllDocumentsAndQuit();

    /* Return true if use selected a font file, false if canceled.*/
    bool
    doNativeOpenFileDialog(FileTypeFilter selectedTypeFilter = FileTypeFilter::All);

    bool
    doQuickOpenFontDialog();
    
    bool
    openFontFile(const QString & filePath);

    bool
    openFontURI(const QXFontURI & uri, FXPtr<FXFace> initFace = nullptr, bool forceNewWindowInstance = false);

protected:
protected:
    void
    addToRecents(QXDocument * document);

private:
    void
    showOpenFontFileError(const QString & file, FXPtr<FXFace> initFace);

    QXDocumentWindow *
    activeDocumentWindow() const;

    QList<QXDocumentWindow * >
    documentWindows() const;

    int
    newFontInstanceId(const QXFontURI & uri);
    
private:
    enum {kMaxRecentFiles = 20};

    QList<QXRecentFontItem> recentFonts_;

#ifdef Q_OS_MAC
    QMenu * openRecentSubMenu;
#endif
    QList<QWidget *>                     managedWindows_ {};
    QMap<QWidget *, QXDocument *>        windowToDocumentMap_ {};
    QList<QXDocument *>                  documents_ {};
    bool                                 appIsAboutToQuit_ {false};
    QXFontListWindow                   * fontListWindow_ {nullptr};
    static QXDocumentWindowManager     * instance_;

};

