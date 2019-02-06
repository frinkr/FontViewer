#include <QHBoxLayout>
#include <QFormLayout>
#include <QSplitter>
#include <QSlider>

#include "FontX/FXFT.h"
#include "FontX/FXLib.h"

#include "QXConv.h"
#include "QXDocument.h"
#include "QXVariableWidget.h"
#include "ui_QXVariableWidget.h"

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
    if (document_->face()->variableAxises().size())
        initVariableFont();
    else
        initNonVariableFont();
}

void
QXVariableWidget::initVariableFont() {

    for (auto & instance : document_->face()->variableNamedInstances()) {
        ui_->instanceComboBox->addItem(toQString(instance.name));
    }

    QFormLayout * layout = new QFormLayout(ui_->axisesWidget);
    for (auto & axis : document_->face()->variableAxises()) {
        QLabel * label = new QLabel(toQString(axis.name), this);
        QSlider * slider = new QSlider(Qt::Horizontal, this);
        layout->addRow(label, slider);
        slider->setMinimum(axis.minValue);
        slider->setMaximum(axis.maxValue);
        slider->setValue(axis.defaultValue);
//        slider->setTracking(true);
        connect(slider, &QSlider::valueChanged, this, &QXVariableWidget::updateFaceVariables);
        sliders_.append(slider);
    }
    ui_->axisesWidget->setLayout(layout);
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

void
QXVariableWidget::updateFaceVariables() {
    FXVector<FXFixed> coords;
    for (auto v : faceVariables()) 
        coords.push_back(v);
    document_->face()->setCurrentVariableCoordinates(coords);

    emit document_->variableCoordinatesChanged();
}

QList<int>
QXVariableWidget::faceVariables() const {
    QList<int> values;
    for (const auto slider: sliders_) 
        values.append(slider->value());
    return values;
}
