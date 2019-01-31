#include "QXSearchEngine.h"
#include "QXDocument.h"
#include "QXSearchWidget.h"
#include "ui_QXSearchWidget.h"

QXSearchWidget::QXSearchWidget(QWidget *parent)
    : QWidget(parent)
    , ui_(new Ui::QXSearchWidget)
    , document_(nullptr) {
    ui_->setupUi(this);

    connect(ui_->lineEdit, &QLineEdit::returnPressed,
            this, &QXSearchWidget::doSearch);
    connect(ui_->lineEdit, &QLineEdit::textChanged,
            this, &QXSearchWidget::onSearchTextChanged);

    labelText_ = ui_->label->text();
}

QXSearchWidget::~QXSearchWidget() {
    delete ui_;
}

void
QXSearchWidget::setDocument(QXDocument * document) {
    document_ = document;
    connect(document_, &QXDocument::searchDone,
            this, &QXSearchWidget::onSearchResult);
}

void
QXSearchWidget::focusInEvent(QFocusEvent *event) {
    QWidget::focusInEvent(event);
    ui_->lineEdit->setFocus();
    ui_->lineEdit->selectAll();
}

void
QXSearchWidget::doSearch() {
    if (document_)
        document_->search(ui_->lineEdit->text());
}

void
QXSearchWidget::onSearchResult(const QXSearchResult & result, const QString & text) {
    if (!result.found)
        ui_->label->setText(tr("No glyph matches \"%1\"").arg(text));
}

void
QXSearchWidget::onSearchTextChanged(const QString & text) {
    ui_->label->setText(labelText_);
}
