#include "QXTTCDialog.h"
#include "ui_QXTTCDialog.h"

QXTTCDialog::QXTTCDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::QXTTCDialog)
{
    ui->setupUi(this);
}

QXTTCDialog::~QXTTCDialog()
{
    delete ui;
}
