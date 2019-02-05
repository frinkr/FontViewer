#include <QHBoxLayout>

#include "FontX/FXFT.h"
#include "FontX/FXLib.h"

#include "QXConv.h"
#include "QXDocument.h"
#include "QXVariableWidget.h"
#include "ui_QXFontVariableWidget.h"

namespace {
    
}

QXVariableWidget::QXVariableWidget(QWidget *parent) :
    QWidget(parent),
    ui_(new Ui::QXVariableWidget) {
    ui_->setupUi(this);
}

QXVariableWidget::~QXVariableWidget() {
    delete ui_;
}

void
QXVariableWidget::setDocument(QXDocument * document) {
    document_ = document;
    if (document->face()->attributes().isOpenTypeVariable || document->face()->attributes().isMultipleMaster)
        initVariableFont();
    else
        initNonVariableFont();
}

void
QXVariableWidget::initVariableFont() {
    return;
    const auto & names = document_->face()->attributes().names;
    FT_Face ftFace = document_->face()->face();
    FT_MM_Var *var = nullptr;
    if (FT_Get_MM_Var(ftFace, &var))
        return;

    // Only OTVar has named instance
    if (FT_IS_VARIATION(ftFace)) {
        for (auto i = 0; i < var->num_namedstyles; ++ i) {
            FT_Var_Named_Style & style = var->namedstyle[i];
            VariableInstance instance;
            instance.name = names.getSFNTName(style.strid);
            instance.psName = names.getSFNTName(style.psid);
            for (auto j = 0; j < var->num_axis; ++ j) 
                instance.coords.push_back(style.coords[j] / 64.0);
            varInstances_.push_back(instance);
        }
    }

    for (auto i = 0; i < var->num_axis; ++ i) {
        FT_Var_Axis & ax = var->axis[i];
    }

    for (const auto & instance: varInstances_) {
        ui_->instanceComboBox->addItem(toQString(instance.name));
    }

    //FT_Done_MM_Var(FXLib::get(), var);


}

void
QXVariableWidget::initNonVariableFont() {
    ui_->instanceLabel->hide();
    ui_->instanceComboBox->hide();

    QHBoxLayout * layout = new QHBoxLayout(ui_->axisesWidget);
    QLabel * warning = new QLabel(tr("Not an OpenType variable or multiple master font!"), ui_->axisesWidget);
    layout->addWidget(warning);
    ui_->axisesWidget->setLayout(layout);
}
