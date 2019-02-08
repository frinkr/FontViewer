#pragma once

#include <QList>
#include <QObject>
#include <QPointer>
#include <QStringList>

#include "QXDocument.h"
#include "QXPreferences.h"

class QXDocumentWindow;
class QXOpenFontDialog;
class QMenu;
class QString;

class QXDocumentWindowManager : public QObject
{
    Q_OBJECT

public:
    QXDocumentWindowManager();
    ~QXDocumentWindowManager();

    static QXDocumentWindowManager *
    instance();

    void
    addDocument(QXDocument * document);

    void
    removeDocument(QXDocument * document);

    const QList<QPointer<QXDocument> > &
    documents() const;

    QXDocument *
    getDocument(const QXFontURI & fontURI) const;
    
    QXDocumentWindow *
    getDocumentWindow(const QXDocument * document) const;

    QXDocumentWindow *
    createDocumentWindow(QXDocument * document);

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
    doOpenFontFromFile();

    void
    openFontFile(const QString & filePath);

    void
    openFontURI(const QXFontURI & uri);

private slots:
    void
    onDocumentWindowAboutToClose(QXDocumentWindow * window);

    void
    onDocumentWindowDestroyed(QObject * obj);

protected:
    void
    removeDocumentWindow(QXDocumentWindow *w);

protected:
    void
    addToRecents(QXDocument * document);

private slots:
    
#ifdef Q_OS_MAC
    void
    slotShowWindow();
#endif

private:
    void
    showOpenFontFileError(const QString & file);

private:
    enum {kMaxRecentFiles = 20};

    QList<QXRecentFontItem> recentFonts_;

#ifdef Q_OS_MAC
    QMenu * openRecentSubMenu;
#endif
    QList<QPointer<QXDocumentWindow> > documentWindows_;
    QList<QPointer<QXDocument> >       documents_;
    bool                               quitRequested_ {false};
    QXOpenFontDialog                 * openFontDialog_ {nullptr};

    static QXDocumentWindowManager   * instance_;

};

