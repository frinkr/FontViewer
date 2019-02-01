#include "FontX/FXVersion.h"
#include "QXConv.h"
#include "QXHtmlTemplate.h"
#include "QXAboutDialog.h"
#include "ui_QXAboutDialog.h"

QXAboutDialog::QXAboutDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::QXAboutDialog) {
    ui->setupUi(this);

    QXHtmlTemplate * html = QXHtmlTemplate::createFromString(ui->textBrowser->toHtml());
    QMap<QString, QVariant> map;
    map["FREETYPE_VERSION"] = toQString(FXVersion::freetype());
    map["HARFBUZZ_VERSION"] = toQString(FXVersion::harfbuzz());
    map["BOOST_VERSION"] = toQString(FXVersion::boost());
    map["ICU_VERSION"] = toQString(FXVersion::icu());
    map["QT_VERSION"] = toQString(QT_VERSION_STR);

    ui->textBrowser->setHtml(html->instantialize(map));
//    setFixedSize(width(), height());
    setWindowFlags(Qt::Dialog | Qt::WindowTitleHint | Qt::CustomizeWindowHint | Qt::WindowCloseButtonHint);
}

QXAboutDialog::~QXAboutDialog() {
    delete ui;
}

void
QXAboutDialog::showAbout() {
    QXAboutDialog dialog;
    dialog.exec();
}
