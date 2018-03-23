#include <QtGui>
#include <QtDebug>
#include <QPlainTextEdit>
#include <QVBoxLayout>
#include <QMenu>
#include <QMenuBar>
#include <QFileDialog>
#include <QMessageBox>
#include <QAbstractButton>

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

    ui_->fileLabel->setText(document->uri().filePath);
}

QUDocumentWindow::~QUDocumentWindow() {
    delete ui_;
}
