#pragma once
#include <QPushButton>

class QXIconPushButton : public QPushButton {
    Q_OBJECT
public:
    using QPushButton::QPushButton;

protected:
    void
    paintEvent(QPaintEvent * event) override;
};
