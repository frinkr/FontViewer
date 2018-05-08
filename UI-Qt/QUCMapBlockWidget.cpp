#include <QStandardItemModel>
#include "QUConv.h"
#include "QUDocument.h"
#include "QUCMapBlockWidget.h"
#include "QUGlyphListView.h"
#include "ui_QUCMapBlockWidget.h"

QUCMapBlockWidget::QUCMapBlockWidget(QWidget *parent)
    : QWidget(parent)
    , ui_(new Ui::QUCMapBlockWidget)
    , document_(nullptr){
    ui_->setupUi(this);

    
}

QUCMapBlockWidget::~QUCMapBlockWidget() {
    delete ui_;
}

QUDocument *
QUCMapBlockWidget::document() const {
    return document_;
}

void
QUCMapBlockWidget::setDocument(QUDocument * document) {
    if (document_) {
        ui_->cmapComboBox->disconnect(document_);
        ui_->blockComboBox->disconnect(document_);
        document_->disconnect(this);
    }
        
    document_ = document;

    connect(ui_->cmapComboBox, QOverload<int>::of(&QComboBox::activated),
            document_, &QUDocument::selectCMap);
    
    connect(document_, &QUDocument::cmapActivated,
            this, &QUCMapBlockWidget::reloadBlocks);
    
    connect(ui_->blockComboBox, QOverload<int>::of(&QComboBox::activated),
            document_, &QUDocument::selectBlock);
    
    connect(document_, &QUDocument::blockSelected,
            ui_->blockComboBox, &QComboBox::setCurrentIndex);

    connect(ui_->glyphCheckBox, &QCheckBox::stateChanged,
            this, &QUCMapBlockWidget::onGlyphMode);

    connect(document_, &QUDocument::charModeActivated,
            this, &QUCMapBlockWidget::onCharMode);

    reloadCMaps();
}

void
QUCMapBlockWidget::reloadCMaps() {
    for (const auto & cmap : document_->face()->cmaps())
        ui_->cmapComboBox->addItem(toQString(cmap.description()), cmap.isValid());
    
    ui_->cmapComboBox->setCurrentIndex(document_->face()->currentCMapIndex());

    // disable invalid cmaps
    QStandardItemModel * model = qobject_cast<QStandardItemModel*>(ui_->cmapComboBox->model());
    for (int i = 0; i < ui_->cmapComboBox->count(); ++ i) {
        QStandardItem* item= model->item(i);
        bool isValid = ui_->cmapComboBox->itemData(i).toBool();
        if (!isValid) 
            item->setFlags(item->flags() & ~Qt::ItemIsEnabled);
    }

    reloadBlocks();
}

void
QUCMapBlockWidget::reloadBlocks() {
    ui_->blockComboBox->clear();
        
    FXCMap cmap = document_->face()->currentCMap();
    for (const auto & block: cmap.blocks()) 
        ui_->blockComboBox->addItem(toQString(block->name()));
}

void
QUCMapBlockWidget::onGlyphMode(bool state) {
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
QUCMapBlockWidget::onCharMode(bool charMode) {
    ui_->glyphCheckBox->setCheckState(charMode? Qt::Unchecked: Qt::Checked);
}
    
