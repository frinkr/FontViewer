#include "FontX/FXInspector.h"
#include "QXConv.h"
#include "QXDocument.h"
#include "QXShapingFeaturesWidget.h"
#include "ui_QXShapingFeaturesWidget.h"

namespace {
    QVariant
    langSysToVariant(FXTag script, FXTag language) {
        return (qulonglong(script) << 32) + language;
    }

    bool
    variantToLangSys(const QVariant & v, FXTag & script, FXTag & language) {
        uint64_t i = v.value<uint64_t>();
        script = (i >> 32);
        language = (i & 0xFFFFFFFF);
        return true;
    }

}
QXShapingFeaturesWidget::QXShapingFeaturesWidget(QWidget *parent) :
    QWidget(parent),
    ui_(new Ui::QXShapingFeaturesWidget) {
    ui_->setupUi(this);

    connect(ui_->langSysComboBox, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, &QXShapingFeaturesWidget::reloadFeatureList);
    connect(ui_->featureListWidget, &QListWidget::itemChanged,
            this, &QXShapingFeaturesWidget::featuresChanged);    
    
}

QXShapingFeaturesWidget::~QXShapingFeaturesWidget() {
    delete ui_;
}

void
QXShapingFeaturesWidget::setDocument(QXDocument * document) {
    document_ = document;
    reloadScriptList();
}

FXVector<FXTag>
QXShapingFeaturesWidget::onFeatures() const {
    FXVector<FXTag> features;
    for (int i = 0; i < ui_->featureListWidget->count(); ++ i) {
        QListWidgetItem * item = ui_->featureListWidget->item(i);
        int state = item->data(Qt::UserRole + 1).value<int>();
        if (state == 1)
            features.push_back(item->data(Qt::UserRole).value<FXTag>());
    }
    features.push_back('locl');
    return features;
}

FXVector<FXTag>
QXShapingFeaturesWidget::offFeatures() const {
    FXVector<FXTag> features;
    for (int i = 0; i < ui_->featureListWidget->count(); ++ i) {
        QListWidgetItem * item = ui_->featureListWidget->item(i);
        int state = item->data(Qt::UserRole + 1).value<int>();
        if (state == -1)
            features.push_back(item->data(Qt::UserRole).value<FXTag>());
    }
    return features;
}

std::tuple<FXTag, FXTag>
QXShapingFeaturesWidget::scriptAndLanguage() const {
    FXTag script, language;
    variantToLangSys(ui_->langSysComboBox->currentData(), script, language);
    return {script, language};
}

void
QXShapingFeaturesWidget::reloadScriptList() {
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
QXShapingFeaturesWidget::reloadFeatureList() {
    FXTag script, language;
    variantToLangSys(ui_->langSysComboBox->currentData(), script, language);
    const FXVector<FXTag> features =inspector()->otFeatures(script, language);

    //QFont font = QFontDatabase::systemFont(QFontDatabase::FixedFont);
    //font.setPointSizeF(font.pointSizeF() * 1.5);
    
    ui_->featureListWidget->clear();
    for (FXTag feature : features) {
        QListWidgetItem * item = new QListWidgetItem(QString::fromStdU16String(u"\u3000 ") + toQString(FXTag2Str(feature)));
        item->setData(Qt::UserRole, feature);
        item->setData(Qt::UserRole + 1, 0);
        item->setData(Qt::ToolTipRole, toQString(FXOT::featureName(feature)));
        //item->setFont(font);
        ui_->featureListWidget->addItem(item);
    }

    connect(ui_->featureListWidget, &QListWidget::itemClicked, [this](QListWidgetItem * item) {
            int state = item->data(Qt::UserRole + 1).value<int>();
            state = (state + 2) % 3 - 1;
        
            item->setData(Qt::UserRole + 1, state);
            
            QString text = item->text();
            text[0] = state? (state == 1? u'\uFF0B': u'\uFF0D'): u'\u3000';
            item->setText(text);
            
        });
    emit featuresChanged();
}


FXPtr<FXInspector>
QXShapingFeaturesWidget::inspector() {
    return document_->face()->inspector();
}
