#include <QFileInfo>
#include <QLineEdit>
#include <QToolButton>
#include <QWidgetAction>
#include "QUConv.h"
#include "QUDocumentWindowManager.h"
#include "QUDocumentWindow.h"
#include "QUGlyphListView.h"
#include "QUToolBarWidget.h"
#include "ui_QUDocumentWindow.h"

QUDocumentWindow::QUDocumentWindow(QUDocument * document, QWidget *parent) 
    : QMainWindow(parent)
    , ui_(new Ui::QUDocumentWindow)
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
    initToolBar();
    initListView();
    
    connectSingals();
}

void
QUDocumentWindow::initWindowTitle() {
    const QString & filePath = document_->uri().filePath;
    ui_->fileLabel->setText(filePath);
    setWindowFilePath(filePath);
    setWindowTitle(QFileInfo(filePath).fileName());
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

    toolBar->addAction(QIcon(":/images/shape.png"), tr("Shape"));
    toolBar->addAction(QIcon(":/images/table.png"), tr("Table"));
    toolBar->addAction(QIcon(":/images/search.png"), tr("Search"));
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
QUDocumentWindow::connectSingals() {
    connect(cmapCombobox_, QOverload<int>::of(&QComboBox::activated),
            document_, &QUDocument::selectCMap);
    
    connect(document_, &QUDocument::cmapActivated,
            this, &QUDocumentWindow::reloadBlocks);

    connect(blockCombobox_, QOverload<int>::of(&QComboBox::activated),
            document_, &QUDocument::selectBlock);
    
    connect(ui_->actionFull_Glyph_List, &QAction::toggled,
            this, &QUDocumentWindow::showFullGlyphList);

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
    document_->setCharMode(!state);
}
