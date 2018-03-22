/*
  Permission for the use of this code is granted only for research, educational, and non-commercial purposes.

  Redistribution of this code or its parts in source, binary, and any other form without permission, with or without modification, is prohibited.  Modifications include, but are not limited to, translation to other programming languages and reuse of tables, constant definitions, and API's defined in it.

  Andrew Choi is not liable for any losses or damages caused by the use of this code.

  Copyright 2009 Andrew Choi.
*/

#ifndef QUDOCUMENTWINDOW_H
#define QUDOCUMENTWINDOW_H

#include <QMainWindow>

class QCloseEvent;
class QPlainTextEdit;
class QMenu;
class QAction;
class QFileDialog;
class QMessageBox;

class QUDocumentWindowManager;

class QUDocumentWindow : public QMainWindow
{
    friend class QUDocumentWindowManager;

    Q_OBJECT

public:
    QUDocumentWindow(QWidget *parent = 0);

    static QUDocumentWindow *createUntitled(int seqNum);
    static QUDocumentWindow *createFromFile(const QString &fn);

    void definitelyClose();  // called in DocumentWindowManager::closeDocumentsAndQuit()

private slots:
    void save();
    void saveAs();

    void finishedSaveAs(int);
    void finishedMaybeSave(int);

    void minimize();
    void zoom();

    void slotAboutToShowFileMenu();
    void slotAboutToShowEditMenu();

#ifdef Q_OS_MAC
    void slotAboutToShowWindowMenu();
#endif

#ifdef Q_OS_MAC
signals:
    void destroyed();
#endif

private:
    void setCurrFile(const QString &fn);

    bool loadFile(const QString &fn);
    bool saveFile(const QString &fn);

    void closeEvent(QCloseEvent *e);

    void saveAndClose();

    void maybeSave();

    void createActions();
    void createMenus();

    QString currFile;
    bool isUntitled;

    QPlainTextEdit *textEdit;

    QMenu *fileMenu;
#ifdef Q_OS_MAC
    QMenu *openRecentSubMenu;
#endif
    QMenu *editMenu;
#ifdef Q_OS_MAC
    // A Window menu really only makes sense on a Mac since it has that single menubar at the top of the screen.
    QMenu *windowMenu;
#endif
    QMenu *helpMenu;

    QAction *newAction;
    QAction *openAction;
    QAction *closeAction;
    QAction *saveAction;
    QAction *saveAsAction;
    QAction *quitAction;

    QAction *undoAction;
    QAction *redoAction;
    QAction *cutAction;
    QAction *copyAction;
    QAction *pasteAction;
    QAction *deleteAction;
    QAction *selectAllAction;

#ifdef Q_OS_MAC
    QAction *minimizeAction;
    QAction *zoomAction;
    QAction *bringAllToFrontAction;
#endif

    QAction *aboutAction;
    QAction *helpAction;

    QFileDialog *saveAsFileDialog;
    QMessageBox *saveMessageBox;

    bool shouldDefinitelyClose;
    bool shouldCloseAfterSaveAs;
};

#endif // DOCUMENTWINDOW_H
