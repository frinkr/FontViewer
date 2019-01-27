#include "QUAboutDialog.h"
#include "ui_QUAboutDialog.h"

QUAboutDialog::QUAboutDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::QUAboutDialog) {
    ui->setupUi(this);
}

QUAboutDialog::~QUAboutDialog() {
    delete ui;
}

void
QUAboutDialog::showAbout() {
    QUAboutDialog dialog;
    dialog.exec();
}
