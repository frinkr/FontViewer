#include <QWidget>
#include <QVariant>

#import <AppKit/AppKit.h>
#import "MacHelper.h"


void
MacHelper::hideTitleBar(QWidget * widget) {
    if (!widget->property("QXMacTitleBarHidden").isNull())
        return;

    NSView *nativeView = reinterpret_cast<NSView *>(widget->winId());
    NSWindow* nativeWindow = nativeView.window;

    QColor background = widget->palette().color(QPalette::Window);

    nativeWindow.styleMask =
        NSWindowStyleMaskTitled |
        NSWindowStyleMaskClosable |
        NSWindowStyleMaskMiniaturizable |
        NSWindowStyleMaskResizable |
        NSWindowStyleMaskFullSizeContentView;

    nativeWindow.titlebarAppearsTransparent = true;

    nativeWindow.backgroundColor = [NSColor colorWithRed:background.redF()
                                                   green:background.greenF()
                                                    blue:background.blueF()
                                                   alpha:1];
    widget->setProperty("QXMacTitleBarHidden", true);
}

