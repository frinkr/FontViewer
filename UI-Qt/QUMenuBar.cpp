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

        menuRecent = menuFile->addMenu(tr("Open &Recent"));
        connect(menuRecent, &QMenu::aboutToShow, [this]() {
            QUDocumentWindowManager::instance()->aboutToShowRecentMenu(menuRecent);
            foreach (QAction * action, menuRecent->actions()) {
                if (!action->isSeparator() && !action->menu()) {
                    connect(action, &QAction::triggered, [action]() {
                        QVariant data = action->data();
                        if (data.canConvert<QUFontURI>()) {
                            QUFontURI uri = data.value<QUFontURI>();
                            QUDocumentWindowManager::instance()->openFontURI(uri);
                        }
                    });
                }
            }
        });

        actionQuit = menuFile->addAction(tr("&Quit"), []() {
            QUDocumentWindowManager::instance()->closeAllDocumentsAndQuit();
        }, QKeySequence(QKeySequence::Quit));
        actionQuit->setMenuRole(QAction::QuitRole);
    }

    menuEdit = addMenu(tr("&Edit")); {
        actionCopy = menuEdit->addAction(tr("&Copy"), [this]() {
            emit copyActionTriggered(actionCopy);
        }, QKeySequence(QKeySequence::Copy));
    }

    menuView = addMenu(tr("&View")); {
        actionCharacterCode = menuView->addAction(tr("&Character Code"));
        actionCharacterCode->setCheckable(true);
        actionGlyphName = menuView->addAction(tr("&Glyph Name"));
        actionGlyphName->setCheckable(true);
        actionGlyphID = menuView->addAction(tr("Glyph &ID"));
        actionGlyphID->setCheckable(true);

        menuView->addSeparator();

#ifndef Q_OS_MAC
        actionFullScreen = menuView->addAction(tr("&Full Screen"), [this]() {
            emit fullScreenActionTriggered(actionFullScreen);
        }, QKeySequence::FullScreen);
        actionFullScreen->setCheckable(true);
#else
        actionFullScreen = nullptr;
#endif
        actionShowAllGlyphs = menuView->addAction(tr("&Show All Glyphs"), [this]() {
            emit showAllGlyphsActionTiggered(actionShowAllGlyphs);
        }, QKeySequence(Qt::CTRL | Qt::Key_G));

        actionShowAllGlyphs->setCheckable(true);

        QActionGroup * group = new QActionGroup(this);
        group->addAction(actionCharacterCode);
        group->addAction(actionGlyphName);
        group->addAction(actionGlyphID);
        group->setExclusive(true);
        actionGlyphName->setChecked(true);

        connect(group, &QActionGroup::triggered, [this](QAction * action) {
            QUGlyphLabel label = QUGlyphLabel::GlyphName;
            if (action == actionCharacterCode)
                label = QUGlyphLabel::CharCode;
            else if (action == actionGlyphName)
                label = QUGlyphLabel::GlyphName;
            else if (action == actionGlyphID)
                label = QUGlyphLabel::GlyphID;

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
            QUDocumentWindowManager::instance()->aboutToShowWindowMenu(menuWindow);
        });
    }

    menuHelp = addMenu(tr("&Help")); {
        actionAbout = menuHelp->addAction(tr("About"), quApp, &QUApplication::about);
        actionAbout->setMenuRole(QAction::AboutRole);
    }
}
