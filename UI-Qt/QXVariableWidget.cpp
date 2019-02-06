#include <QAbstractButton>
#include <QHBoxLayout>
#include <QFormLayout>
#include <QSplitter>
#include <QSlider>
#include <QSignalBlocker>

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
    for (auto & instance : document_->face()->variableNamedInstances())
        ui_->instanceComboBox->addItem(toQString(instance.name), static_cast<int>(instance.index));
    ui_->instanceComboBox->addItem(tr("<current>"), -1);

    connect(ui_->instanceComboBox, QOverload<int>::of(&QComboBox::currentIndexChanged), 
        this, &QXVariableWidget::onComboboxIndexChanged);
            
    QFormLayout * layout = new QFormLayout(ui_->axisesWidget);
    for (auto & axis : document_->face()->variableAxises()) {
        QLabel * label = new QLabel(toQString(axis.name), this);
        QSlider * slider = new QSlider(Qt::Horizontal, this);
        layout->addRow(label, slider);
        slider->setMinimum(axis.minValue);
        slider->setMaximum(axis.maxValue);
        slider->setValue(axis.defaultValue);
        slider->setPageStep((axis.maxValue - axis.minValue) / 20);
        slider->setTracking(true);
        connect(slider, &QSlider::valueChanged, this, &QXVariableWidget::onSliderValueChanged);
        sliders_.append(slider);
    }
    ui_->axisesWidget->setLayout(layout);

    connect(ui_->buttonBox, &QDialogButtonBox::clicked, this, &QXVariableWidget::onResetButtonClicked);

    updateSliderValues();
    updateComboBoxIndex();
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
QXVariableWidget::onComboboxIndexChanged(int index) {
    auto & all = document_->face()->variableNamedInstances();
    if (index < all.size()) {
        auto & instance = all[index];
        if (document_->face()->setCurrentVariableCoordinates(instance.coordinates)) {
            emit document_->variableCoordinatesChanged();
            updateSliderValues();
        }
    }
}

void
QXVariableWidget::onSliderValueChanged() {
    FXVector<FXFixed> coords;
    for (auto v : faceVariables()) 
        coords.push_back(v);
    if (document_->face()->setCurrentVariableCoordinates(coords)) {
        emit document_->variableCoordinatesChanged();
        updateComboBoxIndex();
    }
}

void
QXVariableWidget::onResetButtonClicked() {
    document_->face()->resetVariableCoordinates();
    emit document_->variableCoordinatesChanged();
    updateComboBoxIndex();
    updateSliderValues();
}

QList<int>
QXVariableWidget::faceVariables() const {
    QList<int> values;
    for (const auto slider: sliders_) 
        values.append(slider->value());
    return values;
}

void
QXVariableWidget::updateComboBoxIndex() {
    const auto & instances = document_->face()->variableNamedInstances();
    auto coords = document_->face()->currentVariableCoordinates();

    QSignalBlocker signalBlocker(ui_->instanceComboBox);

    int current = -1;
    for (int i = 0; i < ui_->instanceComboBox->count(); ++ i) {
        QVariant d = ui_->instanceComboBox->itemData(i);
        bool ok = false;
        int index = d.toInt(&ok);
        if (ok) {
            if (index != -1) {
                if (instances[index].coordinates == coords) {
                    ui_->instanceComboBox->setCurrentIndex(i);
                    return;
                }
            }
            else {
                current = i;
            }
        }
    }

    if (current != -1)
        ui_->instanceComboBox->setCurrentIndex(current);
}

void
QXVariableWidget::updateSliderValues() {
    auto coords = document_->face()->currentVariableCoordinates();
    for (size_t i = 0; i < coords.size(); ++ i) {
        QSignalBlocker signalBlocker(sliders_[i]);
        sliders_[i]->setValue(coords[i]);
    } 
}
