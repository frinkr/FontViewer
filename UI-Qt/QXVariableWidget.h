#pragma once

#include <QWidget>
#include "FontX/FX.h"

class QXDocument;

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

    
private:
    Ui::QXVariableWidget * ui_;
    QXDocument               * document_;
    FXVector<VariableInstance> varInstances_;
};
