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

    const QList<QUFontURI> &
    recentFonts() const;

public slots:
    void
    doOpenFontDialog();

    void
    closeAllDocumentsAndQuit();

    void
    openFile(const QString &fn);

private slots:
    void
    slotDocumentWindowDestroyed(QObject * obj);

private:
    QList<QPointer<QUDocumentWindow> > documentWindows_;
    QList<QPointer<QUDocument> > documents_; 
    static QUDocumentWindowManager * instance_;

    //////////////////////////////////////////////////////////////////////////
    
protected:
    void
    removeDocumentWindow(QUDocumentWindow *w);

protected:
    void
    addToRecents(const QUFontURI & uri);

private slots:
    void slotOpenFile();

    void about();
    void help();

private slots:
    
#ifdef Q_OS_MAC
    void slotShowWindow();
    void slotAboutToShowFileMenu();
#endif

    void saveRecentFilesSettings();

private:
    enum {kMaxRecentFiles = 8};
    QList<QUFontURI> recentFonts_;

#ifdef Q_OS_MAC
    QMenu *openRecentSubMenu;
#endif

};

#endif // QUDOCUMENTWINDOWMANAGER_H
