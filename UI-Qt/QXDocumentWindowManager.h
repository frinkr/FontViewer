#pragma once

#include <QList>
#include <QObject>
#include <QPointer>
#include <QStringList>

#include "QXDocument.h"

class QXDocumentWindow;
class QXOpenFontDialog;
class QMenu;
class QString;

struct QXRecentFontItem : public QXFontURI {
    QString fullName;

    friend QDataStream &
    operator << (QDataStream & arch, const QXRecentFontItem & item)
    {
        arch << static_cast<const QXFontURI&>(item);
        arch << item.fullName;
        return arch;
    }

    friend QDataStream &
    operator >> (QDataStream & arch, QXRecentFontItem & item)
    {
        arch >> static_cast<QXFontURI&>(item);
        arch >> item.fullName;
        return arch;
    }
};

Q_DECLARE_METATYPE(QXRecentFontItem);

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

    void doOpenFontFromFile();

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

    void
    loadRecentFontSettings();

    void
    saveRecentFontsSettings();

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

