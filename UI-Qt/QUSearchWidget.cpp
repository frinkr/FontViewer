#include <QTimer>
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
    connect(ui_->lineEdit, &QLineEdit::textChanged,
            this, &QUSearchWidget::onSearchTextChanged);

    labelText_ = ui_->label->text();
}

QUSearchWidget::~QUSearchWidget() {
    delete ui_;
}

void
QUSearchWidget::setDocument(QUDocument * document) {
    document_ = document;
    connect(document_, &QUDocument::searchNotFound,
            this, &QUSearchWidget::onSearchNotFound);
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

void
QUSearchWidget::onSearchNotFound(const QString & text) {
    ui_->label->setText(tr("No glyph matches \"%1\"").arg(text));
}

void
QUSearchWidget::onSearchTextChanged(const QString & text) {
    ui_->label->setText(labelText_);
}
