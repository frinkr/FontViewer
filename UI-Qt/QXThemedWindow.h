#pragma once

#include <QWidget>

class QXWindowDecorator : public QObject{
    Q_OBJECT
public:
    using QObject::QObject;

    virtual void
    onWidgetInit(QWidget * widget) {};

    virtual void
    onWidgetShow(QWidget * widget, QShowEvent * event) {};

    static QXWindowDecorator *
    createInstance(QObject * parent = nullptr);
};

template <class Widget>
class QXThemedWindow : public Widget {
public:
    using Widget::Widget;
protected:
    void
    showEvent(QShowEvent * event) override {
        Widget::showEvent(event);
        if (decorator()) decorator()->onWidgetShow(this, event);
    }

    virtual QXWindowDecorator *
    decorator() {
        if (decorator_ == (QXWindowDecorator *)(-1))
            decorator_ = QXWindowDecorator::createInstance(this);
        return decorator_;
    }
protected:
    QXWindowDecorator   * decorator_ {(QXWindowDecorator *)(-1)};
};
