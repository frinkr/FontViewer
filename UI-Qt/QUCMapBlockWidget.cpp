#include "QUCMapBlockWidget.h"
#include "ui_QUCMapBlockWidget.h"

QUCMapBlockWidget::QUCMapBlockWidget(QWidget *parent)
    : QWidget(parent)
    , ui_(new Ui::QUCMapBlockWidget) {
    ui_->setupUi(this);
}

QUCMapBlockWidget::~QUCMapBlockWidget() {
    delete ui_;
}
