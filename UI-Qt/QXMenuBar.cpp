#include "QXMenuBar.h"
#include "QXApplication.h"
#include "QXDocumentWindowManager.h"

QXMenuBar::QXMenuBar(QWidget * parent)
    : QMenuBar(parent)
{
    menuFile = addMenu(tr("&File")); {
        actionOpen = menuFile->addAction(tr("&Open"), []() {
            QXDocumentWindowManager::instance()->doOpenFontDialog();
        }, QKeySequence(QKeySequence::Open));

        actionOpenFromFile = menuFile->addAction(tr("Open from &File"), []() {
            QXDocumentWindowManager::instance()->doOpenFontFromFile(QXDocumentWindowManager::FileTypeFilter::Font);
        }, QKeySequence(Qt::CTRL | Qt::SHIFT | Qt::Key_O));

        actionOpenFromPDF = menuFile->addAction(tr("Open Embedded Font from PDF"), [] {
            QXDocumentWindowManager::instance()->doOpenFontFromFile(QXDocumentWindowManager::FileTypeFilter::PDF);
        });
        
        actionClose = menuFile->addAction(tr("&Close"), [parent]() {
            if (parent) parent->close();
        }, QKeySequence(QKeySequence::Close));

        menuFile->addSeparator();

        menuRecent = menuFile->addMenu(tr("Open &Recent"));
        connect(menuRecent, &QMenu::aboutToShow, [this]() {
            QXDocumentWindowManager::instance()->aboutToShowRecentMenu(menuRecent);
            foreach (QAction * action, menuRecent->actions()) {
                if (!action->isSeparator() && !action->menu()) {
                    connect(action, &QAction::triggered, [action]() {
                        QVariant data = action->data();
                        if (data.canConvert<QXFontURI>()) {
                            QXFontURI uri = data.value<QXFontURI>();
                            QXDocumentWindowManager::instance()->openFontURI(uri);
                        }
                    });
                }
            }
        });

        actionPreferences = menuFile->addAction(tr("&Preferences"), []() {
            qApp->preferences();
        }, QKeySequence(QKeySequence::Preferences));

        actionQuit = menuFile->addAction(tr("&Quit"), []() {
            QXDocumentWindowManager::instance()->closeAllDocumentsAndQuit();
        }, QKeySequence(QKeySequence::Quit));
        actionQuit->setMenuRole(QAction::QuitRole);
    }

    menuEdit = addMenu(tr("&Edit")); {
        actionCopy = menuEdit->addAction(tr("&Copy"));
        actionCopy->setShortcut(QKeySequence::Copy);

        actionSearch = menuEdit->addAction(tr("&Find"));
        actionSearch->setShortcut(QKeySequence::Find);
    }

    menuView = addMenu(tr("&View")); {

        actionToolBar = menuView->addAction(tr("&Tool Bar"));
        actionToolBar->setCheckable(true);
        actionToolBar->setChecked(true);
        actionStatusBar = menuView->addAction(tr("&Status Bar"));
        actionStatusBar->setCheckable(true);
        actionStatusBar->setChecked(true);

        menuView->addSeparator();

        actionCharacterCode = menuView->addAction(tr("&Character Code"));
        actionCharacterCode->setCheckable(true);
        actionGlyphName = menuView->addAction(tr("&Glyph Name"));
        actionGlyphName->setCheckable(true);
        actionGlyphID = menuView->addAction(tr("Glyph &ID"));
        actionGlyphID->setCheckable(true);

        menuView->addSeparator();

#ifndef Q_OS_MAC
        actionFullScreen = menuView->addAction(tr("&Full Screen"));
        actionFullScreen->setShortcut(QKeySequence::FullScreen);
        actionFullScreen->setCheckable(true);
#else
        actionFullScreen = nullptr;
#endif
        actionShowAllGlyphs = menuView->addAction(tr("&Show All Glyphs"));
        actionShowAllGlyphs->setShortcut(QKeySequence(Qt::CTRL | Qt::Key_G));

        actionShowAllGlyphs->setCheckable(true);

        QActionGroup * group = new QActionGroup(this);
        group->addAction(actionCharacterCode);
        group->addAction(actionGlyphName);
        group->addAction(actionGlyphID);
        group->setExclusive(true);
        actionGlyphName->setChecked(true);

        connect(group, &QActionGroup::triggered, [this](QAction * action) {
            QXGlyphLabel label = QXGlyphLabel::GlyphName;
            if (action == actionCharacterCode)
                label = QXGlyphLabel::CharCode;
            else if (action == actionGlyphName)
                label = QXGlyphLabel::GlyphName;
            else if (action == actionGlyphID)
                label = QXGlyphLabel::GlyphID;

            emit glyphLabelActionTriggered(action, label);
        });
        

    }

    menuWindow = addMenu(tr("&Window")); {
        actionMinimize = menuWindow->addAction(tr("&Minimize"), [parent]() {
            if (parent)
                parent->showMinimized();
        }, QKeySequence(Qt::CTRL | Qt::Key_M));

        actionMaximize = menuWindow->addAction(tr("&Zoom"), [parent]() {
            if (parent)
                parent->showMaximized();
        });

        menuWindow->addSeparator();

        connect(menuWindow, &QMenu::aboutToShow, [this]() {
            QXDocumentWindowManager::instance()->aboutToShowWindowMenu(menuWindow);
        });
    }

    menuHelp = addMenu(tr("&Help")); {
        actionAbout = menuHelp->addAction(tr("About"), qApp, &QXApplication::about);
        actionAbout->setMenuRole(QAction::AboutRole);

        actionAboutFonts = menuHelp->addAction(tr("About Font Database"), qApp, &QXApplication::aboutFonts);
        actionAboutFonts->setMenuRole(QAction::ApplicationSpecificRole);
    }
}
