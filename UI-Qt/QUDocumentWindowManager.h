/*
  Permission for the use of this code is granted only for research, educational, and non-commercial purposes.

  Redistribution of this code or its parts in source, binary, and any other form without permission, with or without modification, is prohibited.  Modifications include, but are not limited to, translation to other programming languages and reuse of tables, constant definitions, and API's defined in it.

  Andrew Choi is not liable for any losses or damages caused by the use of this code.

  Copyright 2009 Andrew Choi.
*/

#ifndef DOCUMENTWINDOWMANAGER_H
#define DOCUMENTWINDOWMANAGER_H

#include <QObject>

#include <QList>
#include <QPointer>
#include <QStringList>

class QUDocumentWindow;
class QMenu;

class QString;

class QUDocumentWindowManager : public QObject
{
    Q_OBJECT

public:
    QUDocumentWindowManager();

    static QUDocumentWindowManager *instance();

    void removeDocumentWindow(QUDocumentWindow *w);
    bool eventFilter(QObject *watched, QEvent *event);
    void addRecentFilesMenuActions(QMenu *recentFilesMenu);

#ifdef Q_OS_MAC
    void addWindowMenuActions(QMenu *windowMenu, QUDocumentWindow *currentWindow);
#endif

    void addToRecentFiles(const QString &fn);
    void removeNonExistingRecentFiles();
    QStringList recentFileDisplayNames();

    int countModifiedDocs();
    void forceCloseAllDocs();

    void continueQuit();
    void cancelQuit();

public slots:
    void newFile();
    void open();

    void openFile(const QString &fn);

#ifdef Q_OS_MAC
    void bringAllToFront();
#endif

    void about();
    void help();
    void closeDocumentsAndQuit();

private slots:
    void slotOpenFile();
#ifdef Q_OS_MAC
    void slotShowWindow();

    void slotAboutToShowFileMenu();
#endif

    void closeAllUnmodifiedDocsAndInitiateQuit();
    void saveRecentFilesSettings();

signals:
    void quitNotPending(bool);

private:
    void initiateQuit();
    void closeNextDoc();

#ifdef Q_OS_MAC
    void cascade(QUDocumentWindow *w);
#endif

    QList<QPointer<QUDocumentWindow> > windows;

    static QUDocumentWindowManager *_instance;

    enum {kMaxRecentFiles = 8};
    QStringList recentFiles;

#ifdef Q_OS_MAC
    QMenu *openRecentSubMenu;
#endif

    bool noDocOpened;
    bool onlyFirstUntitledDocOpened;

    bool quitPending;
};

#endif // DOCUMENTWINDOWMANAGER_H
