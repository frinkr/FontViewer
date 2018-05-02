#include "QUDocument.h"
#include "QUSearchWidget.h"
#include "ui_QUSearchWidget.h"

QUSearchWidget::QUSearchWidget(QWidget *parent)
    : QWidget(parent)
    , ui_(new Ui::QUSearchWidget)
    , document_(nullptr) {
    ui_->setupUi(this);

    connect(ui_->lineEdit, &QLineEdit::returnPressed,
            this, &QUSearchWidget::doSearch);
}

QUSearchWidget::~QUSearchWidget() {
    delete ui_;
}

void
QUSearchWidget::setDocument(QUDocument * document) {
    document_ = document;
}

void
QUSearchWidget::focusInEvent(QFocusEvent *event) {
    QWidget::focusInEvent(event);
    ui_->lineEdit->setFocus();
    ui_->lineEdit->selectAll();
}

void
QUSearchWidget::doSearch() {
    if (document_)
        document_->search(ui_->lineEdit->text());
}
    
