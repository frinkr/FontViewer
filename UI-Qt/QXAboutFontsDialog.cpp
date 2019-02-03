#include "QXAboutFontsDialog.h"
#include "ui_QXAboutFontsDialog.h"

QXAboutFontsDialog::QXAboutFontsDialog(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::QXAboutFontsDialog) {
    ui->setupUi(this);
}

QXAboutFontsDialog::~QXAboutFontsDialog() {
    delete ui;
}

void
QXAboutFontsDialog::showAbout() {
    QXAboutFontsDialog dialog;
    dialog.exec();
}
