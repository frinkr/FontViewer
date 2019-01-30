#include <QFontMetrics>

#include "QUProgressDialog.h"
#include "ui_QUProgressDialog.h"

QUProgressDialog::QUProgressDialog(QWidget * parent) :
    QDialog(parent),
    ui_(new Ui::QUProgressDialog) {
    ui_->setupUi(this);
    setMaximumWidth(400);
    setMinimumWidth(400);
}

QUProgressDialog::~QUProgressDialog() {
    delete ui_;
}

void
QUProgressDialog::setProgress(int value, int maximum, const QString & message) {
    show();
    ui_->progressBar->setMaximum(maximum);
    ui_->progressBar->setValue(value);

    QFontMetrics metrics(ui_->label->font());
    ui_->label->setText(metrics.elidedText(message, Qt::ElideMiddle, maximumWidth()));
    qApp->processEvents();
}
