#include "QUCMapBlockWindow.h"
#include "ui_QUCMapBlockWindow.h"

QUCMapBlockWindow::QUCMapBlockWindow(QWidget *parent) :
    QUPopoverWindow(parent),
    ui_(new Ui::QUCMapBlockWindow) {
    ui_->setupUi(this);
}

QUCMapBlockWindow::~QUCMapBlockWindow() {
    delete ui_;
}
