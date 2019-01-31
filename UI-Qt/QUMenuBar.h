#pragma once

#include <QtWidgets/QMenu>
#include <QtWidgets/QMenuBar>

#include "QUDocument.h"

class QUMenuBar : public QMenuBar {
    Q_OBJECT
public:

    
    // File menu
    QAction * actionOpen;
    QAction * actionOpenFromFile;
    QAction * actionClose;
    QAction * actionRecent;
    QAction * actionPreferences;
    QAction * actionQuit;

    // Edit menu
    QAction * actionCopy;

    // View menu
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


    QMenu * menuFile;
    QMenu * menuRecent;
    QMenu * menuEdit;
    QMenu * menuView;
    QMenu * menuWindow;
    QMenu * menuHelp;

    QUMenuBar(QWidget * parent = nullptr);

signals:

    void
    recentFontActionTriggered(QAction * action, const QUFontURI & uri);

    void
    copyActionTriggered(QAction * action);

    void
    fullScreenActionTriggered(QAction * action);

    void
    showAllGlyphsActionTiggered(QAction * action);

    void
    glyphLabelActionTriggered(QAction * action, QUGlyphLabel label);
};
