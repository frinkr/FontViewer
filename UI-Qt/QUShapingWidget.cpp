#include <QPainter>

#include "FontX/FXInspector.h"
#include "FontX/FXShaper.h"

#include "QUConv.h"
#include "QUDocument.h"
#include "QUShapingWidget.h"
#include "ui_QUShapingWidget.h"

namespace {
    QVariant
    langSysToVariant(FXTag script, FXTag language) {
        return (uint64_t(script) << 32) + language;
    }
    
    bool
    variantToLangSys(const QVariant & v, FXTag & script, FXTag & language) {
        uint64_t i = v.value<uint64_t>();
        script = (i >> 32);
        language = (i & 0xFFFFFFFF);
        return true;
    }
}

QUShapingGlyphView::QUShapingGlyphView(QWidget * parent)
    : QWidget(parent)
    , shaper_(nullptr) {}

void
QUShapingGlyphView::setShaper(FXShaper * shaper) {
    shaper_ = shaper;
}

void
QUShapingGlyphView::paintEvent(QPaintEvent * event) {
    QPainter p(this);
    QRect bounds = rect();
    p.fillRect(bounds, Qt::white);
}

    
QUShapingWidget::QUShapingWidget(QWidget *parent)
    : QWidget(parent)
    , ui_(new Ui::QUShapingWidget)
    , document_(nullptr)      
    , shaper_(nullptr) {
    ui_->setupUi(this);

    ui_->featureListWidget->setSelectionMode(QAbstractItemView::MultiSelection);
    
    // connect signals
    connect(ui_->langSysComboBox, QOverload<int>::of(&QComboBox::activated),
            this, &QUShapingWidget::reloadFeatureList);
    
    connect(ui_->lineEdit, &QLineEdit::textEdited,
            this, &QUShapingWidget::doShape);
}

QUShapingWidget::~QUShapingWidget() {
    delete shaper_;
    delete ui_;
}

void
QUShapingWidget::setDocument(QUDocument * document) {
    document_ = document;
    delete shaper_;
    shaper_ = new FXShaper(document_->face().get());

    ui_->glyphView->setShaper(shaper_);
    
    reloadScriptList();
}

void
QUShapingWidget::reloadScriptList() {
    ui_->langSysComboBox->clear();
    
    const FXVector<FXTag> scripts = inspector()->otScripts();
    for (FXTag script : scripts) {
        const FXVector<FXTag> languages = inspector()->otLanguages(script);
        for (FXTag language : languages) {
            ui_->langSysComboBox->addItem(
                QString("%1 %2 [%3-%4]").arg(
                    toQString(FXOT::scriptName(script)),
                    toQString(FXOT::languageName(language)),
                    toQString(FXTag2Str(script)),
                    toQString(FXTag2Str(language))),
                langSysToVariant(script, language));
        }
    }

    ui_->langSysComboBox->setCurrentIndex(0);
    reloadFeatureList();
}

void
QUShapingWidget::reloadFeatureList() {
    FXTag script, language;
    variantToLangSys(ui_->langSysComboBox->currentData(), script, language);
    const FXVector<FXTag> features =inspector()->otFeatures(script, language);
    
    ui_->featureListWidget->clear();
    for (FXTag feature : features) {
        QListWidgetItem * item = new QListWidgetItem(toQString(FXTag2Str(feature)));
        item->setData(Qt::UserRole, feature);
        ui_->featureListWidget->addItem(item); 
    }

    doShape();
}

void
QUShapingWidget::doShape() {
    if (!shaper_)
        return;

    FXTag script, language;
    variantToLangSys(ui_->langSysComboBox->currentData(), script, language);

    shaper_->shape(toStdString(ui_->lineEdit->text()),
                   script,
                   language,
                   FXShappingLTR,
                   onFeatures(),
                   offFeatures());

    ui_->glyphView->update();
}

FXPtr<FXInspector>
QUShapingWidget::inspector() {
    return document_->face()->inspector();
}

FXVector<FXTag>
QUShapingWidget::onFeatures() const {
    FXVector<FXTag> features;
    for (int i = 0; i < ui_->featureListWidget->count(); ++ i) {
        QListWidgetItem * item = ui_->featureListWidget->item(i);
        if (item->isSelected()) 
            features.push_back(item->data(Qt::UserRole).value<FXTag>());
    }
    return features;
}

FXVector<FXTag>
QUShapingWidget::offFeatures() const {
    FXVector<FXTag> features;
    for (int i = 0; i < ui_->featureListWidget->count(); ++ i) {
        QListWidgetItem * item = ui_->featureListWidget->item(i);
        if (!item->isSelected()) 
            features.push_back(item->data(Qt::UserRole).value<FXTag>());
    }
    return features;        
}
    
