#include <QFileInfo>
#include <QLineEdit>
#include <QToolButton>
#include <QWidgetAction>
#include <QActionGroup>
#include "QUConv.h"
#include "QUCMapBlockWidget.h"
#include "QUDocumentWindowManager.h"
#include "QUDocumentWindow.h"
#include "QUFontInfoWidget.h"
#include "QUPopoverWindow.h"
#include "QUGlyphListView.h"
#include "QUToolBarWidget.h"
#include "ui_QUDocumentWindow.h"

QUDocumentWindow::QUDocumentWindow(QUDocument * document, QWidget *parent) 
    : QMainWindow(parent)
    , ui_(new Ui::QUDocumentWindow)
    , infoDockWidget_(nullptr)
    , cmapBlockWindow_(nullptr)
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

    ui_->actionFull_Glyph_List->setIcon(QIcon(":/images/glyph_d.png"));

    cmapBlockAction_ = toolBar->addAction(
        QIcon(":/images/variant_d.png"),tr("CMap && Blocks"), 
        this, &QUDocumentWindow::onCMapBlockAction);

    QAction* variant = toolBar->addAction(QIcon(":/images/variant_d.png"), tr("Variant"));
    QAction* shape = toolBar->addAction(QIcon(":/images/shape_d.png"), tr("Shape"));
    QAction* table = toolBar->addAction(QIcon(":/images/table_d.png"), tr("Table"));
    QAction* search = toolBar->addAction(QIcon(":/images/search_d.png"), tr("Search"));
    
    QAction* info = toolBar->addAction(
        QIcon(":/images/info_d.png"), tr("Info"),
        this, &QUDocumentWindow::onFontInfoAction);
    

    QMenu *menu = new QMenu();
    QAction *testAction = new QAction("test menu item", this);
    testAction->setIcon(QIcon(":/images/info_d.png"));
    menu->addAction(testAction);
    menu->addAction(variant);
    menu->addAction(shape);
    menu->addAction(table);
    menu->addAction(search);
    menu->addAction(info);

    QToolButton* toolButton = new QToolButton();
    toolButton->setText("Unicode 2.0, BMP Only");
    toolButton->setIcon(QIcon(":/images/search_d.png"));
    toolButton->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
    toolButton->setMenu(menu);
//    toolButton->setDefaultAction(shape);
    toolButton->setPopupMode(QToolButton::InstantPopup);
    QAction * action = toolBar->addWidget(toolButton);
//    action->setIcon(QIcon(":/images/search_d.png"));
    
    QWidget * spacer = new QWidget;
    spacer->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
    toolBar->addWidget(spacer);
    
    searchEdit_ = new QLineEdit;
    searchEdit_->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);
    searchAction_ = toolBar->addWidget(new QUToolBarWidget(searchEdit_, tr("Search")));
    
    this->setUnifiedTitleAndToolBarOnMac(true);
}

void
QUDocumentWindow::initListView() {
    ui_->listView->setModel(document_->model());
    ui_->listView->setItemDelegate(document_->delegate());
}

void
QUDocumentWindow::initGlyphInfoView() {
    ui_->textBrowser->setQUDocument(document_);
}
    
void
QUDocumentWindow::connectSingals() {
    connect(ui_->actionFull_Glyph_List, &QAction::toggled,
            this, &QUDocumentWindow::showFullGlyphList);

    connect(ui_->action_Full_Screen, &QAction::toggled,
            this, &QUDocumentWindow::onToggleFullScreen);
    
    connect(ui_->listView->selectionModel(), &QItemSelectionModel::selectionChanged,
            this, &QUDocumentWindow::onSelectionChanged);

    connect(ui_->textBrowser, &QUGlyphInfoWidget::charLinkClicked,
            this, &QUDocumentWindow::onCharLinkClicked);
}

void
QUDocumentWindow::showFullGlyphList(bool state) {
    document_->model()->setCharMode(!state);
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
        document_->model()->setGlyphLabel(QUGlyphLabel::CharacterCode);
    else if (ui_->actionGlyph_ID->isChecked())
        document_->model()->setGlyphLabel(QUGlyphLabel::GlyphID);
    else
        document_->model()->setGlyphLabel(QUGlyphLabel::GlyphName);
}

void
QUDocumentWindow::onSelectionChanged(const QItemSelection &selected, const QItemSelection &deselected) {
    if (selected.indexes().size()) {
        QModelIndex index = selected.indexes()[0];
        FXGChar c = document_->model()->charAt(index);
        ui_->textBrowser->setChar(c);
    }
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
    QToolButton * cmapBlockToolButton = qobject_cast<QToolButton*>(ui_->toolBar->widgetForAction(cmapBlockAction_));
    cmapBlockWindow_->showRelativeTo(cmapBlockToolButton, QUPopoverRight);
}
    
void
QUDocumentWindow::onFontInfoAction() {
    if (!infoDockWidget_) {
        infoDockWidget_ = new QDockWidget(tr("Info"), this);
        infoDockWidget_->setWidget(new QUFontInfoWidget(document_->face(), infoDockWidget_));
        addDockWidget(Qt::LeftDockWidgetArea, infoDockWidget_);
    }

    if (infoDockWidget_->isVisible())
        infoDockWidget_->hide();
    else {
        infoDockWidget_->show();
        infoDockWidget_->raise();
    }
}
