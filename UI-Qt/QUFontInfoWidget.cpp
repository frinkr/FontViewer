#include <QVBoxLayout>
#include "QUFontInfoWidget.h"
#include "ui_QUFontInfoWidget.h"

QUFontInfoWidget::QUFontInfoWidget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::QUFontInfoWidget)
{
    ui->setupUi(this);
}

QUFontInfoWidget::~QUFontInfoWidget()
{
    delete ui;
}
