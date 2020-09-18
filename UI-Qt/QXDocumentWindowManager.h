#pragma once

#include <QList>
#include <QObject>
#include <QPointer>
#include <QStringList>

#include "QXDocument.h"
#include "QXPreferences.h"

class QXDocumentWindow;
class QXOpenFontDialog;
class QXFontListWidget;
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
    aboutToShowRecentMenu(QMenu * recentMenu);

public slots:
    void
    doOpenFontDialog();

    void
    autoOpenFontDialog();

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
    openFontURI(const QXFontURI & uri, FXPtr<FXFace> initFace = nullptr);

private slots:
    void
    onDocumentWindowAboutToClose(QXDocumentWindow * window);

    void
    onDocumentWindowDestroyed(QObject * obj);

protected:
protected:
    void
    addToRecents(QXDocument * document);

private:
    void
    showOpenFontFileError(const QString & file);

    QXDocumentWindow *
    activeDocumentWindow() const;

    QList<QXDocumentWindow * >
    documentWindows() const;
    
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
    QXFontListWidget                   * openFontDialog_ {nullptr};

    static QXDocumentWindowManager     * instance_;

};

