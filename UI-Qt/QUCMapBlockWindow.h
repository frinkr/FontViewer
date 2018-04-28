#pragma once

#include "QUPopoverWindow.h"

namespace Ui {
    class QUCMapBlockWindow;
}

class QUCMapBlockWindow : public QUPopoverWindow {
    Q_OBJECT

public:
    explicit QUCMapBlockWindow(QWidget *parent = 0);
    ~QUCMapBlockWindow();

private:
    Ui::QUCMapBlockWindow * ui_;
};


