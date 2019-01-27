#include "FontX/FXVersion.h"
#include "QUConv.h"
#include "QUHtmlTemplate.h"
#include "QUAboutDialog.h"
#include "ui_QUAboutDialog.h"

QUAboutDialog::QUAboutDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::QUAboutDialog) {
    ui->setupUi(this);

    QUHtmlTemplate * html = QUHtmlTemplate::createFromString(ui->textBrowser->toHtml());
    QMap<QString, QVariant> map;
    map["FREETYPE_VERSION"] = toQString(FXVersion::freetype());
    map["HARFBUZZ_VERSION"] = toQString(FXVersion::harfbuzz());
    map["BOOST_VERSION"] = toQString(FXVersion::boost());
    map["ICU_VERSION"] = toQString(FXVersion::icu());
    map["QT_VERSION"] = toQString(QT_VERSION_STR);

    ui->textBrowser->setHtml(html->instantialize(map));
}

QUAboutDialog::~QUAboutDialog() {
    delete ui;
}

void
QUAboutDialog::showAbout() {
    QUAboutDialog dialog;
    dialog.exec();
}
