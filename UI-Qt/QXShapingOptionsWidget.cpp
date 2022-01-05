#include "QXApplication.h"
#include "QXShapingOptionsWidget.h"
#include "ui_QXShapingOptionsWidget.h"

QXShapingOptionsWidget::QXShapingOptionsWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::QXShapingOptionsWidget) {
    ui->setupUi(this);

    connect(ui->fontSizeComboBox, &QComboBox::currentTextChanged,
            this, &QXShapingOptionsWidget::optionsChanged);

    connect(ui->showGlyphsBoundaryCheckBox, &QCheckBox::toggled,
            this, &QXShapingOptionsWidget::optionsChanged);

    connect(ui->glyphSpacingSlider, &QSlider::valueChanged,
            this, &QXShapingOptionsWidget::optionsChanged);

    connect(ui->enableGlyphSpacingCheckBox, &QCheckBox::toggled,
            this, &QXShapingOptionsWidget::optionsChanged);

    connect(ui->forceShapeGIDEncodedTextCheckBox, &QCheckBox::toggled,
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

    connect(ui->bidiResolveUnknownScriptsCheckBox, &QCheckBox::toggled,
            this, &QXShapingOptionsWidget::optionsChanged);

    connect(ui->bidiOverrideScriptsCheckBox, &QCheckBox::toggled,
            this, &QXShapingOptionsWidget::optionsChanged);
    
    connect(ui->copyTextButton, &QPushButton::clicked,
            this, &QXShapingOptionsWidget::copyTextButtonClicked);

    connect(ui->copyHexButton, &QPushButton::clicked,
            this, &QXShapingOptionsWidget::copyHexButtonClicked);

    connect(ui->copyHexCStyleButton, &QPushButton::clicked,
            this, &QXShapingOptionsWidget::copyHexCStyleButtonClicked);
    
    ui->copyTextButton->setIcon(qApp->loadIcon(":/images/truetype.png"));
    ui->copyHexButton->setIcon(qApp->loadIcon(":/images/hex.png"));
    ui->copyHexCStyleButton->setIcon(qApp->loadIcon(":/images/hexc.png"));
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
    opts.showGlyphsBoundary = ui->showGlyphsBoundaryCheckBox->isChecked();

    opts.shapingOpts.glyphSpacing = ui->enableGlyphSpacingCheckBox->isChecked()? ui->glyphSpacingSlider->value() / 100.0: 0;
    opts.shapingOpts.forceShapeGIDEncodedText = ui->forceShapeGIDEncodedTextCheckBox->isChecked();
    
    auto dir = ui->bidiDirectionComboBox->currentIndex();
    
    opts.shapingOpts.bidi.activated = ui->bidiGroupBox->isChecked();
    opts.shapingOpts.bidi.breakOnLevelChange = ui->bidiBreakOnLevelChangeCheckBox->isChecked();
    opts.shapingOpts.bidi.breakOnScriptChange = ui->bidiBreakOnScriptChangeCheckBox->isChecked();
    opts.shapingOpts.bidi.resolveScripts = ui->bidiResolveScriptsCheckBox->isChecked();
    opts.shapingOpts.bidi.resolveUnknownScripts = ui->bidiResolveUnknownScriptsCheckBox->isChecked();
    opts.shapingOpts.bidi.overrideScripts = ui->bidiOverrideScriptsCheckBox->isChecked();
    opts.shapingOpts.bidi.direction = dir == 0? FXBidiDirection::LTR : (dir == 1? FXBidiDirection::RTL: FXBidiDirection::AUTO);
    return opts;
}
    

void
QXShapingOptionsWidget::setFontSize(double fontSize) {
    ui->fontSizeComboBox->setCurrentText(QString("%1").arg(fontSize));
}
    
