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
    
    QUDocument *
    getDocument(const QUFontURI & fontURI) const;
    
    QUDocumentWindow *
    getDocumentWindow(const QUDocument * document) const;

    QUDocumentWindow *
    createDocumentWindow(QUDocument * document);


public slots:
    void
    slotOpenFont();
    
    void
    slotDocumentWindowDestroyed(QObject * obj);

    

private:
    QList<QPointer<QUDocumentWindow> > documentWindows_;
    QList<QPointer<QUDocument> > documents_; 
    static QUDocumentWindowManager * instance_;

    //////////////////////////////////////////////////////////////////////////
    
public:
    void removeDocumentWindow(QUDocumentWindow *w);
    void addRecentFilesMenuActions(QMenu *recentFilesMenu);

    void addToRecentFiles(const QString &fn);
    void removeNonExistingRecentFiles();
    QStringList recentFileDisplayNames();

public slots:
    void slotOpenFile();
    
    void openFile(const QString &fn);
    void closeAllDocumentsAndQuit();

    void about();
    void help();

private slots:
    void slotOpenRecentFile();
#ifdef Q_OS_MAC
    void slotShowWindow();

    void slotAboutToShowFileMenu();
#endif

    void saveRecentFilesSettings();

private:
    enum {kMaxRecentFiles = 8};
    QStringList recentFiles;

#ifdef Q_OS_MAC
    QMenu *openRecentSubMenu;
#endif

};

#endif // QUDOCUMENTWINDOWMANAGER_H
