#include <QDockWidget>
#include <QFileInfo>
#include <QLineEdit>
#include <QToolButton>
#include <QWidgetAction>
#include <QActionGroup>
#include "QUApplication.h"
#include "QUConv.h"
#include "QUCMapBlockWidget.h"
#include "QUDocumentWindowManager.h"
#include "QUDocumentWindow.h"
#include "QUDockTitleBarWidget.h"
#include "QUFontInfoWidget.h"
#include "QUPopoverWindow.h"
#include "QUShapingWidget.h"
#include "QUGlyphListView.h"
#include "QUGlyphInfoWidget.h"
#include "QUGlyphTableWidget.h"
#include "QUToolBarWidget.h"
#include "QUSearchWidget.h"
#include "QUSearchEngine.h"
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
    setWindowTitle(QFileInfo(filePath).fileName());
}

void
QUDocumentWindow::initMenu() {
    QActionGroup * group = new QActionGroup(this);
    group->addAction(ui_->actionCharacter_Code);
    group->addAction(ui_->actionGlyph_ID);
    group->addAction(ui_->actionGlyph_Name);
    group->setExclusive(true);
    ui_->actionGlyph_Name->setChecked(true);


    connect(group, &QActionGroup::triggered,
            this, &QUDocumentWindow::onSwitchGlyphLabel);
}

void
QUDocumentWindow::initToolBar() {
    QToolBar * toolBar = ui_->toolBar;

    //ui_->actionFull_Glyph_List->setIcon(
	//	quApp->loadIcon(":/images/glyph.png"));

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
    
    searchAction_->setShortcut(QKeySequence("Ctrl+F"));
        
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
    connect(ui_->action_Open, &QAction::triggered,
            this, &QUDocumentWindow::onOpenFont);
    
    connect(ui_->actionOpen_From_File, &QAction::triggered,
            this, &QUDocumentWindow::onOpenFontFile);

    connect(ui_->action_Close, &QAction::triggered,
            this, &QUDocumentWindow::onCloseAction);

    connect(ui_->action_Quit, &QAction::triggered,
            this, &QUDocumentWindow::onQuitAction);

    connect(ui_->actionFull_Glyph_List, &QAction::toggled,
            this, &QUDocumentWindow::showFullGlyphList);

    connect(ui_->action_Full_Screen, &QAction::toggled,
            this, &QUDocumentWindow::onToggleFullScreen);
    
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
QUDocumentWindow::onOpenFont() {
    QUDocumentWindowManager::instance()->doOpenFontDialog();
}

void
QUDocumentWindow::onOpenFontFile() {
    QUDocumentWindowManager::instance()->slotOpenFile();
}

void
QUDocumentWindow::onCloseAction() {
    this->close();
}

void
QUDocumentWindow::onQuitAction() {
    QUDocumentWindowManager::instance()->closeAllDocumentsAndQuit();
}

void
QUDocumentWindow::showFullGlyphList(bool state) {
    document_->setCharMode(!state);
}

void
QUDocumentWindow::onToggleFullScreen(bool state) {
    if (state)
        showFullScreen();
    else
        showNormal();
}

void
QUDocumentWindow::onSwitchGlyphLabel() {
    if (ui_->actionCharacter_Code->isChecked())
        document_->setGlyphLabel(QUGlyphLabel::CharCode);
    else if (ui_->actionGlyph_ID->isChecked())
        document_->setGlyphLabel(QUGlyphLabel::GlyphID);
    else
        document_->setGlyphLabel(QUGlyphLabel::GlyphName);
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
        tableDockWidget_->setTitleBarWidget(new QUDockTitleBarWidget(this));
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
        shapingDockWidget_->setTitleBarWidget(new QUDockTitleBarWidget(this));
        addDockWidget(Qt::BottomDockWidgetArea, shapingDockWidget_);    
    }
    toggleDockWidget(shapingDockWidget_);
}

void
QUDocumentWindow::onFontInfoAction() {
    if (!infoDockWidget_) {
        infoDockWidget_ = new QDockWidget(tr("Info"), this);
        infoDockWidget_->setWidget(new QUFontInfoWidget(document_->face(), infoDockWidget_));
        infoDockWidget_->setTitleBarWidget(new QUDockTitleBarWidget(this));
        addDockWidget(Qt::LeftDockWidgetArea, infoDockWidget_);
    }

    toggleDockWidget(infoDockWidget_);
}

void
QUDocumentWindow::onSearchAction() {
    if (!searchWindow_) {
        searchWindow_ = new QUPopoverWindow;
        QUSearchWidget * widget = new QUSearchWidget;
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
    
