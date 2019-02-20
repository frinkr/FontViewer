#include "QXThemedWindow.h"

namespace {
    class QXWindowDecoratorWin32 : public QXWindowDecorator {
    public:
        using QXWindowDecorator::QXWindowDecorator;

        void
        onWidgetInit(QWidget * widget) override {
            return;
        }

        void
        onWidgetShow(QWidget * widget, QShowEvent * event) override {
            
        }

        bool
        onNativeEvent(QWidget * widget, const QByteArray & eventType, void * message, long * result) override {
            return false;
        }
    private:
    };
}

QXWindowDecorator *
QXWindowDecorator::createInstance(QObject * parent) {
    return new QXWindowDecoratorWin32(parent);
}
