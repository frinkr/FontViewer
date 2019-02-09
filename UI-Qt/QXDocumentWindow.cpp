#include <QActionGroup>
#include <QDockWidget>
#include <QDragEnterEvent>
#include <QDropEvent>
#include <QFileIconProvider>
#include <QFileInfo>
#include <QLineEdit>
#include <QMessageBox>
#include <QMimeData>
#include <QToolButton>
#include <QWidgetAction>

#include "QXApplication.h"
#include "QXCMapBlockWidget.h"
#include "QXConv.h"
#include "QXDockTitleBarWidget.h"
#include "QXDocumentWindow.h"
#include "QXDocumentWindowManager.h"
#include "QXFontInfoWidget.h"
#include "QXGlyphInfoWidget.h"
#include "QXGlyphListView.h"
#include "QXGlyphTableWidget.h"
#include "QXMenuBar.h"
#include "QXPopoverWindow.h"
#include "QXSearchEngine.h"
#include "QXSearchWidget.h"
#include "QXShapingWidget.h"
#include "QXTheme.h"
#include "QXVariableWidget.h"

#include "ui_QXDocumentWindow.h"

QXDocumentWindow::QXDocumentWindow(QXDocument * document, QWidget *parent) 
    : QMainWindow(parent)
    , ui_(new Ui::QXDocumentWindow)
    , cmapBlockPopover_(nullptr)
    , variablePopover_(nullptr)
    , shapingDockWidget_(nullptr)
    , tableDockWidget_(nullptr)
    , infoDockWidget_(nullptr)
    , glyphPopover_(nullptr)
    , glyphWidget_(nullptr)
    , document_(document)
{
    ui_->setupUi(this);
    setAttribute(Qt::WA_DeleteOnClose);
    initUI();

    document->setParent(this);
    document_->setCharMode(false);
}

QXDocumentWindow::~QXDocumentWindow() {
    delete ui_;
}

void 
QXDocumentWindow::initUI() {
    initWindowTitle();
    initMenu();
    initToolBar();
    initListView();
    initGlyphInfoView();

    connectSingals();

    // Toggle the action to show palette
    // onFontInfoAction();
}

void
QXDocumentWindow::initWindowTitle() {
    const QString & filePath = document_->uri().filePath;
    setWindowFilePath(filePath);
    setWindowTitle(document_->displayName());

#if defined (Q_OS_MAC)
    QFileInfo fi(filePath);
    setWindowIcon(QFileIconProvider().icon(fi));
#endif
}

void
QXDocumentWindow::initMenu() {
    menuBar_ = new QXMenuBar(this);

    connect(menuBar_, &QXMenuBar::copyActionTriggered, [this](QAction * action) {
        
    });

    connect(menuBar_, &QXMenuBar::searchActionTriggered, [this](QAction * action) {
        this->onSearchAction();
    });

    connect(menuBar_, &QXMenuBar::glyphLabelActionTriggered, [this](QAction * action, QXGlyphLabel label) {
        document_->setGlyphLabel(label);
    });

    connect(menuBar_, &QXMenuBar::showAllGlyphsActionTiggered, [this](QAction * action) {
        document_->setCharMode(!action->isChecked());
    });

    connect(document_, &QXDocument::charModeChanged, [this](bool activated) {
        menuBar_->actionShowAllGlyphs->setChecked(!activated);
    });

    connect(menuBar_, &QXMenuBar::fullScreenActionTriggered, [this](QAction * action) {
        if (action->isChecked())
            showFullScreen();
        else
            showNormal();
    });

    setMenuBar(menuBar_);

#if !defined(Q_OS_MAC)
    QXTheme::current()->applyToMenuBar(menuBar_);
#endif
}

void
QXDocumentWindow::initToolBar() {
    QToolBar * toolBar = ui_->toolBar;

    openFontAction_ = toolBar->addAction(
        qApp->loadIcon(":/images/open-font.png"),tr("Open Font"),
        this, &QXDocumentWindow::onOpenFontAction);

    cmapBlockAction_ = toolBar->addAction(
        qApp->loadIcon(":/images/cmap.png"),tr("CMap"),
        this, &QXDocumentWindow::onCMapBlockAction);

    variableAction_ = toolBar->addAction(
		qApp->loadIcon(":/images/variant.png"), tr("OpenType Variable / Multiple Master"),
        this, &QXDocumentWindow::onVariableAction);

    shapingAction_ = toolBar->addAction(
		qApp->loadIcon(":/images/shape.png"), tr("Shape"),
        this, &QXDocumentWindow::onShapingAction);
    
    tableAction_ = toolBar->addAction(
		qApp->loadIcon(":/images/table.png"), tr("Table"),
        this, &QXDocumentWindow::onTableAction);
    
    infoAction_ = toolBar->addAction(
		qApp->loadIcon(":/images/info.png"), tr("Info"),
        this, &QXDocumentWindow::onFontInfoAction);

    QWidget * spacer = new QWidget(this);
    spacer->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
    toolBar->addWidget(spacer);

    initSearchField();

    variableAction_->setShortcut(QKeySequence(Qt::CTRL | Qt::Key_V));
    shapingAction_->setShortcut(QKeySequence(Qt::CTRL | Qt::Key_S));
    tableAction_->setShortcut(QKeySequence(Qt::CTRL | Qt::Key_T));
    infoAction_->setShortcut(QKeySequence(Qt::CTRL | Qt::Key_I));
    
    toolBar->setToolButtonStyle(Qt::ToolButtonIconOnly);

#if defined(Q_OS_MAC)
    this->setUnifiedTitleAndToolBarOnMac(true);
#else
    QXTheme::current()->applyToToolBar(toolBar);
#endif
}

void
QXDocumentWindow::initSearchField() {
    QMenu * searchHistoryMenu = new QMenu(this);
    searchHistoryMenu->addAction("U+FB01");
    searchHistoryMenu->addAction("fi");

    searchLineEdit_ = new QLineEdit(this);
    searchLineEdit_->setStyleSheet("border-radius: 15px;");
    searchLineEdit_->setMinimumHeight(30);
    searchLineEdit_->setMinimumWidth(200);
    searchLineEdit_->setPlaceholderText(tr("Search..."));
    searchLineEdit_->setClearButtonEnabled(true);
    searchLineEdit_->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Fixed);

    QAction * searchIconAction = new QAction(this);
    searchIconAction->setIcon(qApp->loadIcon(":/images/search.png"));
    searchIconAction->setMenu(searchHistoryMenu);
    searchLineEdit_->addAction(searchIconAction, QLineEdit::LeadingPosition);
    ui_->toolBar->addWidget(searchLineEdit_);
    connect(searchLineEdit_, &QLineEdit::returnPressed, this, &QXDocumentWindow::onSearchLineEditReturnPressed);
}

void
QXDocumentWindow::initListView() {
    ui_->listView->setModel(document_);
    ui_->listView->setItemDelegate(new QXGlyphItemDelegate(this));
    ui_->listView->installEventFilter(this);
    ui_->listView->setAcceptDrops(true);
}

void
QXDocumentWindow::initGlyphInfoView() {
    glyphPopover_ = new QXPopoverWindow(this);
    glyphWidget_  = new QXGlyphInfoWidget(glyphPopover_);
    glyphWidget_->setMinimumSize(300, 400);
    glyphPopover_->setWidget(glyphWidget_);
    glyphWidget_->setQUDocument(document_);
}
    
void
QXDocumentWindow::connectSingals() {
    connect(ui_->listView, &QListView::doubleClicked,
            this, &QXDocumentWindow::onGlyphDoubleClicked);

    connect(glyphWidget_, &QXGlyphInfoWidget::charLinkClicked,
            this, &QXDocumentWindow::onCharLinkClicked);

    connect(document_, &QXDocument::searchDone,
            this, &QXDocumentWindow::onSearchResult);
}

QToolButton *
QXDocumentWindow::senderToolButton() {
    QAction * action = qobject_cast<QAction*>(sender());
    if (action) 
        return qobject_cast<QToolButton*>(ui_->toolBar->widgetForAction(action));
    
    return qobject_cast<QToolButton *>(sender());
}

void
QXDocumentWindow::closeEvent(QCloseEvent * event) {
    emit aboutToClose(this);
    QMainWindow::closeEvent(event);
}

void
QXDocumentWindow::dropEvent(QDropEvent * event) {
    for (const QUrl & url: event->mimeData()->urls()) {
        event->acceptProposedAction();
        QString filePath = url.toLocalFile();
        QXDocumentWindowManager::instance()->openFontFile(filePath);
        return;
    }
    QMainWindow::dropEvent(event);
}

bool
QXDocumentWindow::eventFilter(QObject * watched, QEvent * event) {
    if (watched == ui_->listView || watched == menuBar_) {
        if (event->type() == QEvent::Drop) {
            dropEvent(static_cast<QDropEvent *>(event));
            event->accept();
            return true;
        }
        else if (event->type() == QEvent::DragEnter) {
            auto ev = static_cast<QDragEnterEvent *>(event);
            if (ev->mimeData()->hasUrls())
                ev->acceptProposedAction();
            return true;
        }
    }
    return false;
}

void
QXDocumentWindow::onGlyphDoubleClicked(const QModelIndex &index) {
    FXGChar c = document_->charAt(index);
    glyphWidget_->setChar(c);

    QRect rect = ui_->listView->visualRect(index);
    QRect globalRect(ui_->listView->mapToGlobal(rect.topLeft()),
                     ui_->listView->mapToGlobal(rect.bottomRight()));

    glyphPopover_->showRelativeTo(globalRect, QXPopoverTop);
}

void
QXDocumentWindow::onCharLinkClicked(FXGChar c) {
    //ui_->listView->selectChar(c);
}

void
QXDocumentWindow::onOpenFontAction() {
    QXDocumentWindowManager::instance()->doOpenFontDialog();
}

void
QXDocumentWindow::onCMapBlockAction() {
    if (!cmapBlockPopover_) {
        cmapBlockPopover_ = new QXPopoverWindow(this);
        QXCMapBlockWidget * widget = new QXCMapBlockWidget;
        widget->setDocument(document_);
        cmapBlockPopover_->setWidget(widget);
    }
    cmapBlockPopover_->showRelativeTo(senderToolButton(), QXPopoverRight);
}

void
QXDocumentWindow::onVariableAction() {
    if (!variablePopover_) {
        variablePopover_ = new QXPopoverWindow(this);
        QXVariableWidget * widget = new QXVariableWidget;
        widget->setDocument(document_);
        variablePopover_->setWidget(widget);
    }
    variablePopover_->showRelativeTo(senderToolButton(), QXPopoverBottom);
}

void
QXDocumentWindow::onTableAction() {
    if (!tableDockWidget_) {
        tableDockWidget_ = new QDockWidget(tr("Glyphs Table"), this);
        tableDockWidget_->setWidget(new QUGlyphTableWidget(document_));
        tableDockWidget_->setTitleBarWidget(new QXDockTitleBarWidget(tableDockWidget_));
        addDockWidget(Qt::BottomDockWidgetArea, tableDockWidget_);
    }
    toggleDockWidget(tableDockWidget_);
}

void
QXDocumentWindow::onShapingAction() {
    if (!shapingDockWidget_) {
        shapingDockWidget_ = new QDockWidget(tr("Shape"), this);
        QUShapingWidget * widget = new QUShapingWidget(this);
        widget->setDocument(document_);
        shapingDockWidget_->setWidget(widget);
        shapingDockWidget_->setTitleBarWidget(new QXDockTitleBarWidget(shapingDockWidget_));
        addDockWidget(Qt::BottomDockWidgetArea, shapingDockWidget_);    
    }
    toggleDockWidget(shapingDockWidget_);
}

void
QXDocumentWindow::onFontInfoAction() {
    if (!infoDockWidget_) {
        infoDockWidget_ = new QDockWidget(tr("Info"), this);
        infoDockWidget_->setWidget(new QXFontInfoWidget(document_->face(), infoDockWidget_));
        infoDockWidget_->setTitleBarWidget(new QXDockTitleBarWidget(infoDockWidget_));
        addDockWidget(Qt::LeftDockWidgetArea, infoDockWidget_);
    }

    toggleDockWidget(infoDockWidget_);
}

void
QXDocumentWindow::onSearchAction() {
    searchLineEdit_->setFocus();
    searchLineEdit_->selectAll();
}

void
QXDocumentWindow::onSearchLineEditReturnPressed() {
    document_->search(searchLineEdit_->text());
}

void
QXDocumentWindow::onSearchResult(const QXSearchResult & result, const QString & text) {
    if (!result.found) {
        QMessageBox::warning(this,
                             tr("Failed to find glyph"),
                             tr(R"(Expression "%1" doesn't match any glypyh!)").arg(text));
        return;
    }
        
    document_->setCharMode(result.charMode);
    document_->selectBlock(result.block);

    QModelIndex index = document_->index(result.index, 0);
    ui_->listView->selectionModel()->select(index, QItemSelectionModel::SelectCurrent);
    ui_->listView->scrollTo(index);
    ui_->listView->setFocus();
}

void
QXDocumentWindow::toggleDockWidget(QDockWidget * dockWidget) {
    if (dockWidget->isVisible())
        dockWidget->hide();
    else {
        dockWidget->show();
        dockWidget->raise();
    }
}
    
