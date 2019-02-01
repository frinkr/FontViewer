#include <QActionGroup>
#include <QDockWidget>
#include <QFileIconProvider>
#include <QFileInfo>
#include <QLineEdit>
#include <QToolButton>
#include <QWidgetAction>

#include "QUApplication.h"
#include "QUCMapBlockWidget.h"
#include "QUConv.h"
#include "QUDockTitleBarWidget.h"
#include "QUDocumentWindow.h"
#include "QUDocumentWindowManager.h"
#include "QUFontInfoWidget.h"
#include "QUGlyphInfoWidget.h"
#include "QUGlyphListView.h"
#include "QUGlyphTableWidget.h"
#include "QUMenuBar.h"
#include "QUPopoverWindow.h"
#include "QUSearchEngine.h"
#include "QXSearchWidget.h"
#include "QUShapingWidget.h"
#include "QXTheme.h"

#include "ui_QUDocumentWindow.h"

QUDocumentWindow::QUDocumentWindow(QUDocument * document, QWidget *parent) 
    : QMainWindow(parent)
    , ui_(new Ui::QUDocumentWindow)
    , cmapBlockWindow_(nullptr)
    , shapingDockWidget_(nullptr)
    , tableDockWidget_(nullptr)
    , infoDockWidget_(nullptr)
    , searchWindow_(nullptr)
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

QUDocumentWindow::~QUDocumentWindow() {
    delete ui_;
}

void 
QUDocumentWindow::initUI() {
    initWindowTitle();
    initMenu();
    initToolBar();
    initListView();
    initGlyphInfoView();

    connectSingals();
}

void
QUDocumentWindow::initWindowTitle() {
    const QString & filePath = document_->uri().filePath;
    setWindowFilePath(filePath);
    setWindowTitle(document_->displayName());

#if defined (Q_OS_MAC)
    QFileInfo fi(filePath);
    setWindowIcon(QFileIconProvider().icon(fi));
#endif
}

void
QUDocumentWindow::initMenu() {
    menuBar_ = new QUMenuBar(this);

    connect(menuBar_, &QUMenuBar::copyActionTriggered, [this](QAction * action) {
        
    });

    connect(menuBar_, &QUMenuBar::glyphLabelActionTriggered, [this](QAction * action, QUGlyphLabel label) {
        document_->setGlyphLabel(label);
    });

    connect(menuBar_, &QUMenuBar::showAllGlyphsActionTiggered, [this](QAction * action) {
        document_->setCharMode(!action->isChecked());
    });

    connect(document_, &QUDocument::charModeChanged, [this](bool activated) {
        menuBar_->actionShowAllGlyphs->setChecked(!activated);
    });

    connect(menuBar_, &QUMenuBar::fullScreenActionTriggered, [this](QAction * action) {
        if (action->isChecked())
            showFullScreen();
        else
            showNormal();
    });

    setMenuBar(menuBar_);
    QXTheme::applyDarkFusionOnMenuBar(menuBar_);
    
}

void
QUDocumentWindow::initToolBar() {
    QToolBar * toolBar = ui_->toolBar;

    cmapBlockAction_ = toolBar->addAction(
        quApp->loadIcon(":/images/cmap.png"),tr("CMap"),
        this, &QUDocumentWindow::onCMapBlockAction);

    QAction* variant = toolBar->addAction(
		quApp->loadIcon(":/images/variant.png"), tr("Variant"));

    shapingAction_ = toolBar->addAction(
		quApp->loadIcon(":/images/shape.png"), tr("Shape"),
        this, &QUDocumentWindow::onShapingAction);
    
    tableAction_ = toolBar->addAction(
		quApp->loadIcon(":/images/table.png"), tr("Table"),
        this, &QUDocumentWindow::onTableAction);
    
    infoAction_ = toolBar->addAction(
		quApp->loadIcon(":/images/info.png"), tr("Info"),
        this, &QUDocumentWindow::onFontInfoAction);
    
    searchAction_ = toolBar->addAction(
		quApp->loadIcon(":/images/search.png"), tr("Search"),
        this, &QUDocumentWindow::onSearchAction);
    
    searchAction_->setShortcut(QKeySequence(QKeySequence::Find));
        
    QWidget * spacer = new QWidget;
    spacer->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
    toolBar->addWidget(spacer);
    
    toolBar->setToolButtonStyle(Qt::ToolButtonIconOnly);

    this->setUnifiedTitleAndToolBarOnMac(true);
}

void
QUDocumentWindow::initListView() {
    ui_->listView->setModel(document_);
    ui_->listView->setItemDelegate(new QUGlyphItemDelegate(this));    
}

void
QUDocumentWindow::initGlyphInfoView() {
    glyphPopover_ = new QUPopoverWindow(this);
    glyphWidget_  = new QUGlyphInfoWidget;
    glyphWidget_->setMinimumSize(300, 400);
    glyphPopover_->setWidget(glyphWidget_);
    glyphWidget_->setQUDocument(document_);
}
    
void
QUDocumentWindow::connectSingals() {
    connect(ui_->listView, &QListView::doubleClicked,
            this, &QUDocumentWindow::onGlyphDoubleClicked);

    connect(glyphWidget_, &QUGlyphInfoWidget::charLinkClicked,
            this, &QUDocumentWindow::onCharLinkClicked);

    connect(document_, &QUDocument::searchDone,
            this, &QUDocumentWindow::onSearchResult);
}

QToolButton *
QUDocumentWindow::senderToolButton() {
    QAction * action = qobject_cast<QAction*>(sender());
    if (action) 
        return qobject_cast<QToolButton*>(ui_->toolBar->widgetForAction(action));
    
    return qobject_cast<QToolButton *>(sender());
}

void
QUDocumentWindow::closeEvent(QCloseEvent * event) {
    emit aboutToClose(this);
    QMainWindow::closeEvent(event);
}

void
QUDocumentWindow::onGlyphDoubleClicked(const QModelIndex &index) {
    FXGChar c = document_->charAt(index);
    glyphWidget_->setChar(c);

    QRect rect = ui_->listView->visualRect(index);
    QRect globalRect(ui_->listView->mapToGlobal(rect.topLeft()),
                     ui_->listView->mapToGlobal(rect.bottomRight()));

    glyphPopover_->showRelativeTo(globalRect, QUPopoverTop);
}

void
QUDocumentWindow::onCharLinkClicked(FXGChar c) {
    //ui_->listView->selectChar(c);
}

void
QUDocumentWindow::onCMapBlockAction() {
    if (!cmapBlockWindow_) {
        cmapBlockWindow_ = new QUPopoverWindow(this);
        QUCMapBlockWidget * widget = new QUCMapBlockWidget;
        widget->setDocument(document_);
        cmapBlockWindow_->setWidget(widget);
    }
    cmapBlockWindow_->showRelativeTo(senderToolButton(), QUPopoverRight);
}

void
QUDocumentWindow::onTableAction() {
    if (!tableDockWidget_) {
        tableDockWidget_ = new QDockWidget(tr("Glyphs Table"), this);
        tableDockWidget_->setWidget(new QUGlyphTableWidget(document_));
        tableDockWidget_->setTitleBarWidget(new QUDockTitleBarWidget(tableDockWidget_));
        addDockWidget(Qt::BottomDockWidgetArea, tableDockWidget_);
    }
    toggleDockWidget(tableDockWidget_);
}

void
QUDocumentWindow::onShapingAction() {
    if (!shapingDockWidget_) {
        shapingDockWidget_ = new QDockWidget(tr("Shape"), this);
        QUShapingWidget * widget = new QUShapingWidget(this);
        widget->setDocument(document_);
        shapingDockWidget_->setWidget(widget);
        shapingDockWidget_->setTitleBarWidget(new QUDockTitleBarWidget(shapingDockWidget_));
        addDockWidget(Qt::BottomDockWidgetArea, shapingDockWidget_);    
    }
    toggleDockWidget(shapingDockWidget_);
}

void
QUDocumentWindow::onFontInfoAction() {
    if (!infoDockWidget_) {
        infoDockWidget_ = new QDockWidget(tr("Info"), this);
        infoDockWidget_->setWidget(new QUFontInfoWidget(document_->face(), infoDockWidget_));
        infoDockWidget_->setTitleBarWidget(new QUDockTitleBarWidget(infoDockWidget_));
        addDockWidget(Qt::LeftDockWidgetArea, infoDockWidget_);
    }

    toggleDockWidget(infoDockWidget_);
}

void
QUDocumentWindow::onSearchAction() {
    if (!searchWindow_) {
        searchWindow_ = new QUPopoverWindow;
        QXSearchWidget * widget = new QXSearchWidget;
        widget->setDocument(document_);
        searchWindow_->setWidget(widget);
    }
    searchWindow_->showRelativeTo(senderToolButton(), QUPopoverBottom);
}

void
QUDocumentWindow::onSearchResult(const QUSearchResult & result, const QString & text) {
    if (!result.found)
        return;
    document_->setCharMode(result.charMode);
    document_->selectBlock(result.block);

    QModelIndex index = document_->index(result.index, 0);
    ui_->listView->selectionModel()->select(index, QItemSelectionModel::SelectCurrent);
    ui_->listView->scrollTo(index);
    
    if (searchWindow_)
        searchWindow_->hide();
}

void
QUDocumentWindow::toggleDockWidget(QDockWidget * dockWidget) {
    if (dockWidget->isVisible())
        dockWidget->hide();
    else {
        dockWidget->show();
        dockWidget->raise();
    }
}
    
