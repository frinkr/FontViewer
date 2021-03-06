#pragma once

#include <QtWidgets/QMenu>
#include <QtWidgets/QMenuBar>

#include "QXDocument.h"

class QXMenuBar : public QMenuBar {
    Q_OBJECT
public:

    
    // File menu
    QAction * actionOpen;
    QAction * actionOpenFromFile;
    QAction * actionOpenFromPDF;
    QAction * actionClose;
    QAction * actionCloseAll;
    QAction * actionRecent;
    QAction * actionPreferences;
    QAction * actionQuit;

    // Edit menu
    QAction * actionCopy;
    QAction * actionSearch;

    // View menu
    QAction * actionToolBar;
    QAction * actionStatusBar;
    QAction * actionCharacterCode;
    QAction * actionGlyphName;
    QAction * actionGlyphID;
    QAction * actionFullScreen;
    QAction * actionShowAllGlyphs;

    // Window menu
    QAction * actionMinimize;
    QAction * actionMaximize;
    QAction * actionZoom;

    // Help menu
    QAction * actionAbout;
    QAction * actionAboutFonts;

    QMenu * menuFile;
    QMenu * menuRecent;
    QMenu * menuEdit;
    QMenu * menuView;
    QMenu * menuWindow;
    QMenu * menuHelp;

    QXMenuBar(QWidget * parent = nullptr);

signals:

    void
    recentFontActionTriggered(QAction * action, const QXFontURI & uri);

    void
    glyphLabelActionTriggered(QAction * action, QXGlyphLabel label);
};
