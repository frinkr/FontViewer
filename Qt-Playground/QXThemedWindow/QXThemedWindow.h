#pragma once

#include <QWidget>

bool
QXThemedWindowInit(QWidget * widget);

template <class Widget>
class QXThemedWindow : public Widget {
public:
    QXThemedWindow(QWidget * parent = nullptr, Qt::WindowFlags f = Qt::WindowFlags())
        : Widget(parent, f) {
        QXThemedWindowInit(this);
    }
};
