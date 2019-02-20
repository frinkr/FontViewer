#pragma once
#include <QMessageBox>
#include <QWidget>
#include <type_traits>

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
    template <typename ...Args>
    QXThemedWindow(Args &&... args)
        : Widget(std::forward<Args>(args)...){
        decorator_ = QXWindowDecorator::createInstance(this);
        if (decorator_) 
            decorator_->onWidgetInit(this);
    }

protected:
    void
    showEvent(QShowEvent * event) override {
        Widget::showEvent(event);
        if (decorator_) 
            decorator_->onWidgetShow(this, event);
    }

protected:
    QXWindowDecorator   * decorator_ {nullptr};
};
