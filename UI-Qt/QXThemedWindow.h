#pragma once
#include <QMessageBox>
#include <QWidget>
#include <type_traits>

class QXWindowDecorator : public QObject{
    Q_OBJECT
public:
    using QObject::QObject;

    virtual void
    onWidgetInit(QWidget * widget) {
    }

    virtual void
    onWidgetShow(QWidget * widget, QShowEvent * event) {
    }

    virtual bool
    onNativeEvent(QWidget * widget, const QByteArray & eventType, void * message,  qintptr * result) {
        return false;
    }

    static QXWindowDecorator *
    createInstance(QObject * parent = nullptr);
};

template <class Widget, bool ApplyTheme = true>
class QXThemedWindow : public Widget {
public:
    template <typename ...Args>
    QXThemedWindow(Args &&... args)
        : Widget(std::forward<Args>(args)...){
        if (ApplyTheme) {
            decorator_ = QXWindowDecorator::createInstance(this);
            if (decorator_) 
                decorator_->onWidgetInit(this);
        }
    }

protected:
    void
    showEvent(QShowEvent * event) override {
        Widget::showEvent(event);
        if (ApplyTheme && decorator_) 
            decorator_->onWidgetShow(this, event);
    }

    bool
    nativeEvent(const QByteArray & eventType, void * message,  qintptr * result) override {
        if (ApplyTheme && decorator_ && decorator_->onNativeEvent(this, eventType, message, result))
            return true;

        return Widget::nativeEvent(eventType, message, result);
    }

protected:
    QXWindowDecorator   * decorator_ {nullptr};
};
