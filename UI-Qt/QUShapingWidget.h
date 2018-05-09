#pragma once

#include <QWidget>

namespace Ui {
    class QUShapingWidget;
}

class QUShapingWidget : public QWidget
{
    Q_OBJECT

public:
    explicit QUShapingWidget(QWidget *parent = 0);
    ~QUShapingWidget();

private:
    Ui::QUShapingWidget * ui_;
};


