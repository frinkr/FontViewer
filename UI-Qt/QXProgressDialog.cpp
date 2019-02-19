#include <QFontMetrics>

#include "QXProgressDialog.h"
#include "ui_QXProgressDialog.h"

QXProgressDialog::QXProgressDialog(QWidget * parent) :
    QXThemedWindow<QDialog>(parent),
    ui_(new Ui::QXProgressDialog) {
    ui_->setupUi(this);
    setMaximumWidth(400);
    setMinimumWidth(400);
}

QXProgressDialog::~QXProgressDialog() {
    delete ui_;
}

void
QXProgressDialog::setProgress(int value, int maximum, const QString & message) {
    show();
    ui_->progressBar->setMaximum(maximum);
    ui_->progressBar->setValue(value);

    QFontMetrics metrics(ui_->label->font());
    ui_->label->setText(metrics.elidedText(message, Qt::ElideMiddle, maximumWidth()));
    qApp->processEvents();
}
