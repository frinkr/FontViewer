#include <QAction>
#include <QDragEnterEvent>
#include <QDropEvent>
#include <QLineEdit>
#include <QMenu>
#include <QMimeData>
#include <QTimer>

#include "FontX/FXBenchmark.h"
#include "QXApplication.h"
#include "QXConv.h"
#include "QXDocumentWindowManager.h"
#include "QXFontListWindow.h"
#include "QXFontListModel.h"
#include "QXFontManager.h"
#include "QXPopoverWindow.h"
#include "QXResources.h"
#include "ui_QXFontListWindow.h"

namespace {
    constexpr qreal MIN_PREVIEW_FONT_SIZE = 20;
    constexpr qreal MAX_PREVIEW_FONT_SIZE = 200;
}

QXFontListWindow::QXFontListWindow(QWidget * parent)
    : QXThemedWindow<QMainWindow>(parent)
    , ui_(new Ui::QXFontListWindow) {
    ui_->setupUi(this);

    // List view
    ui_->fontListView->setDb(QXFontManager::instance().db());
    ui_->fontListView->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(ui_->fontListView, &QListView::doubleClicked, this, &QXFontListWindow::acceptFont, Qt::QueuedConnection);

    // List view: select last open font
    QXDocumentWindowManager * mgr = QXDocumentWindowManager::instance();
    if (mgr->recentFonts().size()) {
        for (const auto & uri: mgr->recentFonts()) {
            if (-1 != ui_->fontListView->selectFont(uri))
                break;
        }
    }
    if (-1 == ui_->fontListView->selectedFontIndex())
        ui_->fontListView->selectFont(0);

    // Ok button
    connect(ui_->okButton, &QPushButton::clicked, this, &QXFontListWindow::acceptFont);
        
    // Search Edit
    ui_->searchLineEdit->setStyleSheet(
        QString("QLineEdit { "                                          \
                "  border: 2px solid; "                                 \
                "  border-radius: 14px; "                               \
                "  background-color: palette(base);}"                   \
                "QLineEdit:focus { "                                    \
                "  border: 2px solid; "                                 \
                "  border-radius: 14px; "                               \
                "  border-color: palette(highlight);}"));
    ui_->searchLineEdit->setAttribute(Qt::WA_MacShowFocusRect, 0);
    ui_->searchLineEdit->setMinimumHeight(28);
    ui_->searchLineEdit->setMinimumWidth(200);
    ui_->searchLineEdit->setPlaceholderText(tr("Search..."));
    ui_->searchLineEdit->setClearButtonEnabled(true);
    ui_->searchLineEdit->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    ui_->searchLineEdit->installEventFilter(this);
    
    QAction * searchIconAction = new QAction(this);
    searchIconAction->setIcon(qApp->loadIcon(":/images/search.png"));
    ui_->searchLineEdit->addAction(searchIconAction, QLineEdit::LeadingPosition);
    connect(ui_->searchLineEdit, &QLineEdit::returnPressed, this, &QXFontListWindow::onSearchLineEditReturnPressed);
    connect(ui_->searchLineEdit, &QLineEdit::textEdited, this, &QXFontListWindow::onFilterChanged);

    // Menu button
    ui_->menuButton->setIcon(qApp->loadIcon(":/images/menu.png"));
    QMenu * menu = new QMenu(ui_->menuButton);
    ui_->menuButton->setMenu(menu);

    menu->addAction(qApp->loadIcon(":/images/preview.png"), tr("Preview Options"), [this]() {
            if (ui_->previewSettingsGoupBox->isVisible())
                ui_->previewSettingsGoupBox->hide();
            else
                ui_->previewSettingsGoupBox->show();
        });
    menu->addAction(qApp->loadIcon(":/images/open.png"), tr("Open Font File"), [this]() {
            onOpenFileButtonClicked();
        });
    menu->addSeparator();

    recentMenu_ = menu->addMenu(qApp->loadIcon(":/images/history.png"), tr("Recents"));
    connect(recentMenu_, &QMenu::aboutToShow, [this]() {
        QXDocumentWindowManager::instance()->reloadRecentMenu(recentMenu_, true);
        foreach (QAction * action, recentMenu_->actions()) {
            if (!action->isSeparator() && !action->menu()) {
                connect(action, &QAction::triggered, [this, action]() {
                    QVariant data = action->data();
                    if (data.canConvert<QXFontURI>()) {
                        QXFontURI uri = data.value<QXFontURI>();
                        if (-1 == selectFont(uri)) {
                            QTimer::singleShot(0, [this, uri]() {
                                QXDocumentWindowManager::instance()->openFontURI(uri);
                                hide();
                            });
                        }
                    }
                });
            }
        }
    });

    // Preview settings
    ui_->previewFontSizeSlider->setMinimum(MIN_PREVIEW_FONT_SIZE);
    ui_->previewFontSizeSlider->setMaximum(MAX_PREVIEW_FONT_SIZE);
    ui_->previewFontSizeSlider->setValue(QXFontListViewPreview::defaultPreview().fontSize);
    ui_->previewTextEdit->setText(QXFontListViewPreview::defaultPreview().sampleText);
    connect(ui_->previewFontSizeSlider, &QSlider::valueChanged, this, &QXFontListWindow::updatePreviewText);
    connect(ui_->previewTextEdit, &QLineEdit::textEdited, this, &QXFontListWindow::onPreviewTextChanged);
    connect(ui_->previewCoverAllCharsCheckBox, &QCheckBox::stateChanged, this, &QXFontListWindow::onFilterChanged);
        
    ui_->previewSettingsGoupBox->hide();

    // Add actions
    QAction * searchAction = new QAction(this);
    connect(searchAction, &QAction::triggered, this, &QXFontListWindow::onSearchAction);
    searchAction->setShortcuts(QKeySequence::Find);
    addAction(searchAction);

    qApp->dismissSplashScreen(this);
    
    setWindowTitle(QString("%1 (bm %2s/%3s)")
                   .arg(windowTitle())
                   .arg(QXFontManager::instance().dbInitSeconds(), 0, 'f', 2)
                   .arg(qApp->benchmark().time().count(), 0, 'f', 2));
    //setWindowTitle(QString("%1 (scaning fonts in %2s)").arg(windowTitle()).arg(qApp->benchmark().time().count()));
        
    // Drops
    this->setAcceptDrops(true);
    ui_->fontListView->setAcceptDrops(true);
    ui_->fontListView->installEventFilter(this);
}

QXFontListWindow::~QXFontListWindow() {
    delete ui_;
}

QXFontURI
QXFontListWindow::selectedFont() const {
    return ui_->fontListView->selectedFont();
}

int
QXFontListWindow::selectFont(const QXFontURI & fontURI) {
    return ui_->fontListView->selectFont(fontURI);
}

QLineEdit *
QXFontListWindow::searchLineEdit() const {
    return ui_->searchLineEdit;
}

bool
QXFontListWindow::eventFilter(QObject * obj, QEvent * event) {
    if (obj == ui_->searchLineEdit && event->type() == QEvent::KeyPress) {
        QKeyEvent * keyEvent = (QKeyEvent*)event;

        if ((keyEvent->modifiers() & ~Qt::KeypadModifier) == Qt::NoModifier &&
            (keyEvent->key() == Qt::Key_Down ||
             keyEvent->key() == Qt::Key_PageDown ||
             keyEvent->key() == Qt::Key_Up ||
             keyEvent->key() == Qt::Key_PageUp ||
             keyEvent->key() == Qt::Key_Home ||
             keyEvent->key() == Qt::Key_End)) {

            QKeyEvent * eventCopy = new QKeyEvent(QEvent::KeyPress, keyEvent->key(), Qt::NoModifier);
            qApp->postEvent(ui_->fontListView, eventCopy);
        }
    }
    else if (event->type() == QEvent::DragEnter) {
        if (obj == ui_->fontListView || obj == ui_->searchLineEdit) {
            auto dragEnterEvent = ((QDragEnterEvent *)event);
            if (dragEnterEvent->mimeData()->hasUrls()) {
                dragEnterEvent->acceptProposedAction();
                return true;
            }
        }
    }
    else if (event->type() == QEvent::Drop) {
        if (obj == ui_->fontListView || obj == ui_->searchLineEdit) {
            if (QXDocumentWindowManager::instance()->handleDropEvent((QDropEvent*)event)) {
                event->accept();
                hide();
                return true;
            }
        }
    }

    return QWidget::eventFilter(obj, event);
}

QXFontListFilter
QXFontListWindow::fontListFilter() const {
    QXFontListFilter filter;
    filter.fontName = ui_->searchLineEdit->text().trimmed();
    filter.sampleText = ui_->previewTextEdit->text();
    filter.converAllSampleCharacters = ui_->previewCoverAllCharsCheckBox->checkState() == Qt::Checked;
    return filter;
}

void
QXFontListWindow::acceptFont() {
    hide();
    emit fontSelected(selectedFont());
}

void
QXFontListWindow::onSearchLineEditReturnPressed() {
    acceptFont();
}

void
QXFontListWindow::onFilterChanged() {
    ui_->fontListView->setFilter(fontListFilter());
}

void
QXFontListWindow::onSearchAction() {
    ui_->searchLineEdit->setFocus();
    ui_->searchLineEdit->selectAll();
}

void
QXFontListWindow::onOpenFileButtonClicked() {
    if (QXDocumentWindowManager::instance()->doNativeOpenFileDialog())
        hide();    
}

void
QXFontListWindow::onPreviewTextChanged() {
    updatePreviewText();
    
    if (ui_->previewCoverAllCharsCheckBox->checkState() == Qt::Checked)
        onFilterChanged();
}

void
QXFontListWindow::updatePreviewText() {
    QXFontListViewPreview preview;
    preview.sampleText = ui_->previewTextEdit->text();
    preview.fontSize = ui_->previewFontSizeSlider->value();
    ui_->fontListView->setPreview(preview);
}
