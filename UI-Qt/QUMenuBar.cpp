#include "QUMenuBar.h"
#include "QUApplication.h"
#include "QUDocumentWindowManager.h"

QUMenuBar::QUMenuBar(QWidget * parent)
    : QMenuBar(parent)
{

    menuFile = addMenu(tr("&File")); {
        actionOpen = menuFile->addAction(tr("&Open"), []() {
            QUDocumentWindowManager::instance()->doOpenFontDialog();
        }, QKeySequence(QKeySequence::Open));

        actionOpenFromFile = menuFile->addAction(tr("Open from &File"), []() {
            QUDocumentWindowManager::instance()->doOpenFontFromFile();
        }, QKeySequence(Qt::CTRL | Qt::SHIFT | Qt::Key_O));

        actionClose = menuFile->addAction(tr("&Close"), [parent]() {
            if (parent) parent->close();
        }, QKeySequence(QKeySequence::Close));

        menuFile->addSeparator();

        actionQuit = menuFile->addAction(tr("&Quit"), []() {
            qApp->quit();
        }, QKeySequence(QKeySequence::Quit));
        actionQuit->setMenuRole(QAction::QuitRole);
    }

    menuEdit = addMenu(tr("&Edit")); {
        actionCopy = menuEdit->addAction(tr("&Copy"), []() {
            // TODO:
        }, QKeySequence(QKeySequence::Copy));
    }

    menuHelp = addMenu(tr("&Help")); {
        actionAbout = menuHelp->addAction(tr("About"), quApp, &QUApplication::about);
        actionAbout->setMenuRole(QAction::AboutRole);
    }

#if 0
    actionAbout = new QAction(parent);
    actionAbout->setObjectName(QString::fromUtf8("action_About"));
    actionAbout->setMenuRole(QAction::AboutRole);
    actionAbout_Qt = new QAction(parent);
    actionAbout_Qt->setObjectName(QString::fromUtf8("actionAbout_Qt"));
    actionAbout_Qt->setMenuRole(QAction::AboutQtRole);
    actionOpen = new QAction(parent);
    actionOpen->setObjectName(QString::fromUtf8("action_Open"));
    actionOpenFromFile = new QAction(parent);
    actionOpenFromFile->setObjectName(QString::fromUtf8("action_Open_From_File"));
    actionClose = new QAction(parent);
    actionClose->setObjectName(QString::fromUtf8("action_Close"));
    actionQuit = new QAction(parent);
    actionQuit->setObjectName(QString::fromUtf8("action_Quit"));
    actionQuit->setMenuRole(QAction::QuitRole);
    actionCopy = new QAction(parent);
    actionCopy->setObjectName(QString::fromUtf8("action_Copy"));
    actionFullScreen = new QAction(parent);
    actionFullScreen->setObjectName(QString::fromUtf8("action_Full_Screen"));
    actionFullScreen->setCheckable(true);
    actionCharacterCode = new QAction(parent);
    actionCharacterCode->setObjectName(QString::fromUtf8("actionCharacter_Code"));
    actionCharacterCode->setCheckable(true);
    actionGlyphName = new QAction(parent);
    actionGlyphName->setObjectName(QString::fromUtf8("actionGlyph_Name"));
    actionGlyphName->setCheckable(true);
    actionGlyphName->setChecked(false);
    actionGlyphID = new QAction(parent);
    actionGlyphID->setObjectName(QString::fromUtf8("actionGlyph_ID"));
    actionGlyphID->setCheckable(true);
    actionFullGlyphList = new QAction(parent);
    actionFullGlyphList->setObjectName(QString::fromUtf8("actionFull_Glyph_List"));
    actionFullGlyphList->setCheckable(true);
    actionGlyph = new QAction(parent);
    actionGlyph->setObjectName(QString::fromUtf8("actionGlyph"));
    actionGlyph->setCheckable(true);
    actionProperties = new QAction(parent);
    actionProperties->setObjectName(QString::fromUtf8("actionProperties"));
    actionProperties->setCheckable(true);
    actionGlyph_2 = new QAction(parent);
    actionGlyph_2->setObjectName(QString::fromUtf8("actionGlyph_2"));
    actionProperties_2 = new QAction(parent);
    actionProperties_2->setObjectName(QString::fromUtf8("actionProperties_2"));
    actionMinimize = new QAction(parent);
    actionMinimize->setObjectName(QString::fromUtf8("actionMinimize"));
    actionZoom = new QAction(parent);
    actionZoom->setObjectName(QString::fromUtf8("actionZoom"));

    QMenuBar * menuBar = this;
    this->setGeometry(QRect(0, 0, 1094, 22));
    menuFile = new QMenu(menuBar);
    menuFile->setObjectName(QString::fromUtf8("menu_File"));
    menuEdit = new QMenu(menuBar);
    menuEdit->setObjectName(QString::fromUtf8("menu_Edit"));
    menuView = new QMenu(menuBar);
    menuView->setObjectName(QString::fromUtf8("menu_View"));
    menuWindow = new QMenu(menuBar);
    menuWindow->setObjectName(QString::fromUtf8("menu_Window"));
    menuHelp = new QMenu(menuBar);
    menuHelp->setObjectName(QString::fromUtf8("menu_Help"));

    menuBar->addAction(menuFile->menuAction());
    menuBar->addAction(menuEdit->menuAction());
    menuBar->addAction(menuView->menuAction());
    menuBar->addAction(menuWindow->menuAction());
    menuBar->addAction(menuHelp->menuAction());
    menuFile->addAction(actionOpen);
    menuFile->addAction(actionOpenFromFile);
    menuFile->addAction(actionClose);
    menuFile->addSeparator();
    menuFile->addAction(actionQuit);
    menuEdit->addAction(actionCopy);
    menuView->addAction(actionGlyphID);
    menuView->addAction(actionGlyphName);
    menuView->addAction(actionCharacterCode);
    menuView->addSeparator();
    menuView->addAction(actionFullGlyphList);
    menuView->addAction(actionFullScreen);
    menuWindow->addAction(actionMinimize);
    menuWindow->addAction(actionZoom);
    menuHelp->addAction(actionAbout);
#endif
}
