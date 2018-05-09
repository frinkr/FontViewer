#include "QUShapingWidget.h"
#include "ui_QUShapingWidget.h"

QUShapingWidget::QUShapingWidget(QWidget *parent)
    : QWidget(parent)
    , ui_(new Ui::QUShapingWidget) {
    ui_->setupUi(this);
}

QUShapingWidget::~QUShapingWidget() {
    delete ui_;
}
