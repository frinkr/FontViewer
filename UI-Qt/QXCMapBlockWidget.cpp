#include <QStandardItemModel>
#include "QXConv.h"
#include "QXDocument.h"
#include "QXCMapBlockWidget.h"
#include "QXGlyphListView.h"
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
        ui_->blockComboBox->disconnect(document_);
        document_->disconnect(this);
    }
        
    document_ = document;

    connect(ui_->cmapComboBox, QOverload<int>::of(&QComboBox::activated),
            document_, &QXDocument::selectCMap);
    
    connect(document_, &QXDocument::cmapActivated,
            this, &QXCMapBlockWidget::reloadBlocksCombobox);
    
    connect(ui_->blockComboBox, QOverload<int>::of(&QComboBox::activated),
            document_, &QXDocument::selectBlock);
    
    connect(document_, &QXDocument::blockSelected,
            ui_->blockComboBox, &QComboBox::setCurrentIndex);

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
    ui_->blockComboBox->clear();
        
    FXCMap cmap = document_->face()->currentCMap();
    for (const auto & block: cmap.blocks()) 
        ui_->blockComboBox->addItem(toQString(block->name()));
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
    
