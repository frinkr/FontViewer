#include <QLabel>
#include <QVBoxLayout>
#include "QUToolBarWidget.h"

QUToolBarWidget::QUToolBarWidget(QWidget * child, const QString & label, QWidget * parent)
    : QWidget(parent) {
    label_ = new QLabel(label, this);
    label_->setAlignment(Qt::AlignHCenter);
    QVBoxLayout * layout = new QVBoxLayout(this);

    //layout->setSpacing(0);
    int left, top, right, bottom;
    layout->getContentsMargins(&left, &top, &right, &bottom);

    layout->setContentsMargins(left, top, right, 9);
    layout->addWidget(child);
    layout->addWidget(label_);
}

QString 
QUToolBarWidget::label() const {
    return label_->text();
}

void
QUToolBarWidget::setLabel(const QString & label) const {
    return label_->setText(label);
}

