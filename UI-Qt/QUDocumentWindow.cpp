#include <QFileInfo>
#include <QLineEdit>
#include <QToolButton>
#include <QWidgetAction>
#include <QActionGroup>
#include "QUConv.h"
#include "QUDocumentWindowManager.h"
#include "QUDocumentWindow.h"
#include "QUFontInfoWidget.h"
#include "QUGlyphListView.h"
#include "QUToolBarWidget.h"
#include "ui_QUDocumentWindow.h"

QUDocumentWindow::QUDocumentWindow(QUDocument * document, QWidget *parent) 
    : QMainWindow(parent)
    , ui_(new Ui::QUDocumentWindow)
    , infoDockWidget_(nullptr)
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

    // cmap Combobox
    cmapCombobox_ = new QComboBox;
    cmapCombobox_->setFixedWidth(150);
    cmapAction_ = toolBar->addWidget(new QUToolBarWidget(cmapCombobox_, tr("Character Maps")));
    for (const auto & cmap : document_->face()->cmaps())
        cmapCombobox_->addItem(toQString(cmap.description()));

    blockCombobox_ = new QComboBox;
    blockCombobox_->setFixedWidth(200);
    blockAction_ = toolBar->addWidget(new QUToolBarWidget(blockCombobox_, tr("Character Blocks")));
    reloadBlocks();
    
    ui_->actionFull_Glyph_List->setIcon(QIcon(":/images/glyph.png"));
    
    toolBar->addAction(QIcon(":/images/variant_d.png"), tr("Variant"));
    toolBar->addAction(QIcon(":/images/shape_d.png"), tr("Shape"));
    toolBar->addAction(QIcon(":/images/table_d.png"), tr("Table"));
    toolBar->addAction(QIcon(":/images/search_d.png"), tr("Search"));
    toolBar->addAction(QIcon(":/images/info_d.png"), tr("Info"), this, &QUDocumentWindow::onFontInfoAction);
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
    connect(cmapCombobox_, QOverload<int>::of(&QComboBox::activated),
            document_, &QUDocument::selectCMap);
    
    connect(document_, &QUDocument::cmapActivated,
            this, &QUDocumentWindow::reloadBlocks);

    connect(blockCombobox_, QOverload<int>::of(&QComboBox::activated),
            document_->model(), &QUGlyphListModel::selectBlock);
    
    connect(ui_->actionFull_Glyph_List, &QAction::toggled,
            this, &QUDocumentWindow::showFullGlyphList);

    connect(ui_->listView->selectionModel(), &QItemSelectionModel::selectionChanged,
            this, &QUDocumentWindow::onSelectionChanged);

    connect(ui_->textBrowser, &QUGlyphInfoWidget::charLinkClicked,
            this, &QUDocumentWindow::onCharLinkClicked);
}

void
QUDocumentWindow::reloadBlocks() {
    blockCombobox_->clear();
        
    FXCMap cmap = document_->face()->currentCMap();
    for (const auto & block: cmap.blocks()) 
        blockCombobox_->addItem(toQString(block->name()));
}

void
QUDocumentWindow::showFullGlyphList(bool state) {
    cmapAction_->setEnabled(!state);
    blockAction_->setEnabled(!state);
    document_->model()->setCharMode(!state);
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
        FXChar c = document_->model()->charAt(index);
        if (document_->model()->charMode())
            ui_->textBrowser->setChar(c);
        else
            ui_->textBrowser->setGlyph(c);
    }
}
    
void
QUDocumentWindow::onCharLinkClicked(FXChar c) {
    //ui_->listView->selectChar(c);
}

void
QUDocumentWindow::onFontInfoAction() {
    if (!infoDockWidget_) {
        infoDockWidget_ = new QDockWidget(tr("Info"), this);
        infoDockWidget_->setWidget(new QUFontInfoWidget(infoDockWidget_));
        addDockWidget(Qt::LeftDockWidgetArea, infoDockWidget_);
    }

    if (infoDockWidget_->isVisible())
        infoDockWidget_->hide();
    else {
        infoDockWidget_->show();
        infoDockWidget_->raise();
    }
}
