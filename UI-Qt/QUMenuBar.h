#pragma once

#include <QtWidgets/QMenu>
#include <QtWidgets/QMenuBar>

class QUMenuBar : public QMenuBar {
    Q_OBJECT
public:
    QAction *actionAbout;
    

    QAction *actionOpen;
    QAction *actionOpenFromFile;
    QAction *actionClose;
    QAction *actionQuit;

    QAction *actionCopy;
    QAction *actionFullScreen;
    QAction *actionCharacterCode;
    QAction *actionGlyphName;
    QAction *actionGlyphID;
    QAction *actionFullGlyphList;
    QAction *actionGlyph;
    QAction *actionProperties;
    QAction *actionMinimize;
    QAction *actionZoom;

    QMenu *menuFile;
    QMenu *menuEdit;
    QMenu *menuView;
    QMenu *menuWindow;
    QMenu *menuHelp;

    QUMenuBar(QWidget *parent = nullptr);
};
