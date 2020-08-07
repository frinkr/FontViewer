#include "QXShapingOptionsWidget.h"
#include "ui_QXShapingOptionsWidget.h"

QXShapingOptionsWidget::QXShapingOptionsWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::QXShapingOptionsWidget) {
    ui->setupUi(this);

    connect(ui->fontSizeComboBox, &QComboBox::currentTextChanged,
            this, &QXShapingOptionsWidget::optionsChanged);

    connect(ui->bidiGroupBox, & QGroupBox::toggled,
            this, &QXShapingOptionsWidget::optionsChanged);

    connect(ui->bidiDirectionComboBox, &QComboBox::currentTextChanged,
            this, &QXShapingOptionsWidget::optionsChanged);
    
    connect(ui->bidiBreakOnLevelChangeCheckBox, &QCheckBox::toggled,
            this, &QXShapingOptionsWidget::optionsChanged);
    
    connect(ui->bidiBreakOnScriptChangeCheckBox, &QCheckBox::toggled,
            this, &QXShapingOptionsWidget::optionsChanged);
    
    connect(ui->bidiResolveScriptsCheckBox, &QCheckBox::toggled,
            this, &QXShapingOptionsWidget::optionsChanged);
        
    connect(ui->copyTextButton, &QPushButton::clicked,
            this, &QXShapingOptionsWidget::copyTextButtonClicked);
        
    connect(ui->togglePanelPushButton, &QPushButton::clicked,
            this, &QXShapingOptionsWidget::togglePanelButtonClicked);
}

QXShapingOptionsWidget::~QXShapingOptionsWidget() {
    delete ui;
}

QXShapingOptions
QXShapingOptionsWidget::options() const {
    bool ok = false;
    double fontSize = ui->fontSizeComboBox->currentText().toDouble(&ok);
    if (!ok)
        fontSize = 100;
    
    QXShapingOptions opts;
    opts.fontSize = fontSize;
    
    auto dir = ui->bidiDirectionComboBox->currentIndex();
    
    opts.bidi.bidiActivated = ui->bidiGroupBox->isChecked();
    opts.bidi.breakOnLevelChange = ui->bidiBreakOnLevelChangeCheckBox->isChecked();
    opts.bidi.breakOnScriptChange = ui->bidiBreakOnScriptChangeCheckBox->isChecked();
    opts.bidi.resolveScripts = ui->bidiResolveScriptsCheckBox->isChecked();
    opts.bidi.direction = dir == 0? FXBidiDirection::LTR : (dir == 1? FXBidiDirection::RTL: FXBidiDirection::AUTO);
    
    return opts;
}
    
