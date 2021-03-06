#import <AppKit/AppKit.h>
#include <QWidget>
#include <QWindow>
#include "QXThemedWindow.h"

namespace {
    bool
    setupWindowTitleBar(QWidget * widget) {
        NSObject * nativeView = nil;

        QWindow * window = widget->windowHandle();
        if (window)
            nativeView = reinterpret_cast<NSObject *>(window->winId());
        if (!nativeView)
            nativeView = reinterpret_cast<NSObject *>(widget->winId());

        NSWindow * nativeWindow = nil;
        if ([nativeView isKindOfClass:[NSView class]])
            nativeWindow = static_cast<NSView *>(nativeView).window;
        else if ([nativeView isKindOfClass:[NSWindow class]])
            nativeWindow = static_cast<NSWindow *>(nativeView);

        if (nativeWindow == nil)
            return false;

        nativeWindow.titlebarAppearsTransparent = true;
        QColor background = widget->palette().color(QPalette::Window);
        nativeWindow.backgroundColor = [NSColor colorWithRed:background.redF()
                                        green:background.greenF()
                                        blue:background.blueF()
                                        alpha:1];
        return true;
    }


    class QXWindowDecoratorMac: public QXWindowDecorator {
    public: 
        using QXWindowDecorator::QXWindowDecorator;

        void
        onWidgetInit(QWidget * widget) override {
            setupWindowTitleBar(widget);
        }

        void
        onWidgetShow(QWidget * widget, QShowEvent * event) override {
            setupWindowTitleBar(widget);
        }
    };
}

QXWindowDecorator *
QXWindowDecorator::createInstance(QObject * parent) {
    return new QXWindowDecoratorMac(parent);
}

