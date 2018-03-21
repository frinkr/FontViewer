#include "QUMainWindow.h"
#include "ui_qumainwindow.h"

QUMainWindow::QUMainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::QUMainWindow)
{
    ui->setupUi(this);
}

QUMainWindow::~QUMainWindow()
{
    delete ui;
}