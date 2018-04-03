#include <QtGui>
#include <QtDebug>
#include <QCombobox>
#include <QFileInfo>
#include <QLineEdit>
#include <QStringListModel>
#include <QStandardItemModel>

#include "QUDocumentWindowManager.h"
#include "QUDocumentWindow.h"
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
    QComboBox * cmapCombobox = new QComboBox;
    toolBar->addWidget(cmapCombobox);
    toolBar->addAction(QIcon(":/images/copy.png"), "Copy");
    
    QWidget * spacer = new QWidget;
    spacer->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
    toolBar->addWidget(spacer);
    
    QLineEdit * searchEdit = new QLineEdit;
    searchEdit->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);
    toolBar->addWidget(searchEdit);
    
    this->setUnifiedTitleAndToolBarOnMac(true);
}

void
QUDocumentWindow::initListView() {
    ui_->listView->setModel(document_);
    ui_->listView->setLayoutMode(QListView::Batched);
    ui_->listView->setBatchSize(100);
    ui_->listView->setGridSize(QSize(100, 120));
    ui_->listView->setIconSize(QSize(100, 100));
    ui_->listView->setUniformItemSizes(true);
    ui_->listView->setMovement(QListView::Static);
    ui_->listView->setViewMode(QListView::IconMode);
    ui_->listView->setResizeMode(QListView::Adjust);
    ui_->listView->setEditTriggers(QAbstractItemView::NoEditTriggers);
}
    
