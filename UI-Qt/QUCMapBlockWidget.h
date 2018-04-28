#pragma once

#include "QUPopoverWindow.h"

namespace Ui {
    class QUCMapBlockWidget;
}

class QUCMapBlockWidget : public QWidget {
    Q_OBJECT

public:
    explicit QUCMapBlockWidget(QWidget *parent = 0);
    ~QUCMapBlockWidget();

private:
    Ui::QUCMapBlockWidget * ui_;
};


