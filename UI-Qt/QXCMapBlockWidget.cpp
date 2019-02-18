#include <QStandardItemModel>
#include "QXConv.h"
#include "QXDocument.h"
#include "QXCMapBlockWidget.h"
#include "ui_QXCMapBlockWidget.h"

QXCMapBlockWidget::QXCMapBlockWidget(QWidget *parent)
    : QWidget(parent)
    , ui_(new Ui::QXCMapBlockWidget)
    , document_(nullptr){
    ui_->setupUi(this);

    
}

QXCMapBlockWidget::~QXCMapBlockWidget() {
    delete ui_;
}

QXDocument *
QXCMapBlockWidget::document() const {
    return document_;
}

void
QXCMapBlockWidget::setDocument(QXDocument * document) {
    if (document_) {
        ui_->cmapComboBox->disconnect(document_);
        ui_->bookComboBox->disconnect(document_);
        document_->disconnect(this);
    }
        
    document_ = document;

    connect(ui_->cmapComboBox, QOverload<int>::of(&QComboBox::activated),
            document_, &QXDocument::selectCMap);

    connect(document_, &QXDocument::cmapActivated,
            this, &QXCMapBlockWidget::reloadBlocksCombobox);
    
    connect(ui_->bookComboBox, QOverload<int>::of(&QComboBox::activated),
            this, &QXCMapBlockWidget::onBlockComboBoxChanged);

    connect(document_, &QXDocument::bookSelected,
            this, &QXCMapBlockWidget::onDocumentBookSelected);

    connect(ui_->glyphCheckBox, &QCheckBox::stateChanged,
            this, &QXCMapBlockWidget::onGlyphCheckBox);

    connect(document_, &QXDocument::charModeChanged,
            this, &QXCMapBlockWidget::onDocumentCharModeChanged);

    reloadCMapsCombobox();
    onDocumentCharModeChanged(document_->charMode());
}

void
QXCMapBlockWidget::reloadCMapsCombobox() {
    for (const auto & cmap : document_->face()->cmaps())
        ui_->cmapComboBox->addItem(toQString(cmap.description()), cmap.isValid());
    
    ui_->cmapComboBox->setCurrentIndex(document_->face()->currentCMapIndex());

    // disable invalid cmaps
    QStandardItemModel * model = qobject_cast<QStandardItemModel*>(ui_->cmapComboBox->model());
    for (int i = 0; i < ui_->cmapComboBox->count(); ++ i) {
        QStandardItem * item= model->item(i);
        bool isValid = ui_->cmapComboBox->itemData(i).toBool();
        if (!isValid) 
            item->setFlags(item->flags() & ~Qt::ItemIsEnabled);
    }

    reloadBlocksCombobox();
}

void
QXCMapBlockWidget::reloadBlocksCombobox() {
    ui_->bookComboBox->clear();
        
    auto & books = document_->books();
    for (size_t i = 0; i < books.size(); ++ i) {
        const auto & book = books[i];
        if (book.type() == QXGCharBook::CMap)
            ui_->bookComboBox->addItem(book.name(), static_cast<int>(i));
    }

    for (size_t i = 0; i < books.size(); ++ i) {
        const auto & book = books[i];
        if (book.type() == QXGCharBook::FullUnicode)
            ui_->bookComboBox->addItem(book.name(), static_cast<int>(i));
    }

    ui_->bookComboBox->insertSeparator(9999);

    for (size_t i = 0; i < books.size(); ++ i) {
        const auto & book = books[i];
        if (book.type() == QXGCharBook::One)
            ui_->bookComboBox->addItem(book.name(), static_cast<int>(i));
    }
}

void
QXCMapBlockWidget::onBlockComboBoxChanged(int index) {
    QVariant d = ui_->bookComboBox->itemData(index);
    if (d.canConvert<int>()) {
        auto value = d.value<int>();
        document_->selectBook(value);
    }
}

void
QXCMapBlockWidget::onDocumentBookSelected(int book) {
    for (int i = 0; i < ui_->bookComboBox->count(); ++ i) {
        QVariant d = ui_->bookComboBox->itemData(i);
        if (d.canConvert<int>()) {
            auto value = d.value<int>();
            if (value == book) {
                ui_->bookComboBox->setCurrentIndex(i);
                break;
            }
        }   
    }
}

void
QXCMapBlockWidget::onGlyphCheckBox(bool state) {
    const bool charMode = !state;
    document_->setCharMode(charMode);
    foreach (QObject * obj, children()) {
        QWidget * widget = qobject_cast<QWidget*>(obj);
        if (widget && widget != ui_->glyphCheckBox) {
            widget->setEnabled(charMode);
        }
    }
}

void
QXCMapBlockWidget::onDocumentCharModeChanged(bool charMode) {
    ui_->glyphCheckBox->setCheckState(charMode? Qt::Unchecked: Qt::Checked);
}
    
