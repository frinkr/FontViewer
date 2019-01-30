#ifndef QUDOCUMENTWINDOWMANAGER_H
#define QUDOCUMENTWINDOWMANAGER_H

#include <QObject>

#include <QList>
#include <QPointer>
#include <QStringList>

#include "QUDocument.h"

class QUDocumentWindow;


class QMenu;
class QString;

struct QURecentFontItem : public QUFontURI {
    QString fullName;

    friend QDataStream &
    operator << (QDataStream & arch, const QURecentFontItem & item)
    {
        arch << static_cast<const QUFontURI&>(item);
        arch << item.fullName;
        return arch;
    }

    friend QDataStream &
    operator >> (QDataStream & arch, QURecentFontItem & item)
    {
        arch >> static_cast<QUFontURI&>(item);
        arch >> item.fullName;
        return arch;
    }
};

Q_DECLARE_METATYPE(QURecentFontItem);

class QUDocumentWindowManager : public QObject
{
    Q_OBJECT

public:
    QUDocumentWindowManager();

    static QUDocumentWindowManager *
    instance();

    void
    addDocument(QUDocument * document);

    void
    removeDocument(QUDocument * document);

    const QList<QPointer<QUDocument> > &
    documents() const;

    QUDocument *
    getDocument(const QUFontURI & fontURI) const;
    
    QUDocumentWindow *
    getDocumentWindow(const QUDocument * document) const;

    QUDocumentWindow *
    createDocumentWindow(QUDocument * document);

    const QList<QURecentFontItem> &
    recentFonts() const;

    void
    aboutToShowWindowMenu(QMenu * windowMenu);

    void
    aboutToShowRecentMenu(QMenu * recentMenu);

public slots:
    void
    doOpenFontDialog();

    void
    closeAllDocumentsAndQuit();

    void doOpenFontFromFile();

    void
    openFile(const QString & filePath);

    void
    openFont(const QUFontURI & uri);

private slots:
    void
    onDocumentWindowAboutToClose(QUDocumentWindow * window);

    void
    onDocumentWindowDestroyed(QObject * obj);

protected:
    void
    removeDocumentWindow(QUDocumentWindow *w);

protected:
    void
    addToRecents(QUDocument * document);

private slots:
    
#ifdef Q_OS_MAC
    void
    slotShowWindow();

    void
    slotAboutToShowFileMenu();
#endif

    void
    loadRecentFontSettings();

    void
    saveRecentFontsSettings();

private:
    enum {kMaxRecentFiles = 8};

    QList<QURecentFontItem> recentFonts_;

#ifdef Q_OS_MAC
    QMenu * openRecentSubMenu;
#endif
    QList<QPointer<QUDocumentWindow> > documentWindows_;
    QList<QPointer<QUDocument> >       documents_;
    bool                               quitRequested_ {false};
    static QUDocumentWindowManager   * instance_;

};

#endif // QUDOCUMENTWINDOWMANAGER_H
