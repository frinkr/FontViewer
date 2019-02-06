#pragma once

#include <QWidget>
#include "FontX/FX.h"

class QXDocument;
class QSlider;

namespace Ui {
    class QXVariableWidget;
}

class QXVariableWidget : public QWidget {
    Q_OBJECT

public:
    explicit QXVariableWidget(QWidget * parent = nullptr);
    ~QXVariableWidget();

    void
    setDocument(QXDocument * document);

private:
    struct VariableInstance {
        FXString            name;
        FXString            psName;
        FXVector<double>    coords;
    };

private:
    void
    initVariableFont();

    void
    initNonVariableFont();

    void
    onComboboxIndexChanged(int index);

    void
    onSliderValueChanged();

    void
    onResetButtonClicked();

    QList<int>
    faceVariables() const;

    void
    updateComboBoxIndex();

    void
    updateSliderValues();

private:
    Ui::QXVariableWidget * ui_;
    QXDocument               * document_;
    QList<QSlider *>           sliders_;
    FXVector<VariableInstance> varInstances_;
};
