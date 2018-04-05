#include <QFileInfo>
#include <QLineEdit>

#include "QUConv.h"
#include "QUDocumentWindowManager.h"
#include "QUDocumentWindow.h"
#include "QUGlyphListView.h"
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
    toolBar->addWidget(cmapCombobox_);
    for (const auto & cmap : document_->face()->cmaps())
        cmapCombobox_->addItem(toQString(cmap.description()));

    blockCombobox_ = new QComboBox;
    blockCombobox_->setFixedWidth(200);
    toolBar->addWidget(blockCombobox_);
    reloadBlocks();
    
    glyphModelToggle_ = toolBar->addAction(QIcon(":/images/glyph.png"), "All Glyphs");
    glyphModelToggle_->setCheckable(true);

    QWidget * spacer = new QWidget;
    spacer->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
    toolBar->addWidget(spacer);
    
    QLineEdit * searchEdit = new QLineEdit;
    searchEdit->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);
    toolBar->addWidget(searchEdit);
    
    toolBar->setStyleSheet("QToolBar{spacing:4px;}");

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
    
    connect(glyphModelToggle_, &QAction::toggled,
            this, &QUDocumentWindow::slotSetGlyphMode);
}

void
QUDocumentWindow::reloadBlocks() {
    blockCombobox_->clear();
        
    FXCMap cmap = document_->face()->currentCMap();
    for (const auto & block: cmap.blocks()) 
        blockCombobox_->addItem(toQString(block->name()));
}

void
QUDocumentWindow::slotSetGlyphMode(bool state) {
    cmapCombobox_->setEnabled(!state);
    blockCombobox_->setEnabled(!state);
    document_->setCharMode(!state);
}