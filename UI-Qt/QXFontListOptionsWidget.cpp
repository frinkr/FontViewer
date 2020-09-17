#include "QXFontListOptionsWidget.h"
#include "ui_QXFontListOptionsWidget.h"

QXFontListOptionsWidget::QXFontListOptionsWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::QXFontListOptionsWidget)
{
    ui->setupUi(this);
}

QXFontListOptionsWidget::~QXFontListOptionsWidget()
{
    delete ui;
}
