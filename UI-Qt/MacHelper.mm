#include <QWidget>
#include <QVariant>
#include <QWindow>
#import <AppKit/AppKit.h>
#import "MacHelper.h"


void
MacHelper::hideTitleBar(QWidget * widget) {
    if (!widget->property("QXMacTitleBarHidden").isNull())
        ;//return;
    
    NSWindow * nativeWindow = nil;
    NSObject * nativeView = reinterpret_cast<NSObject *>(widget->windowHandle()->winId());
    if ([nativeView isKindOfClass:[NSView class]])
        nativeWindow = static_cast<NSView *>(nativeView).window;
    else if ([nativeView isKindOfClass:[NSWindow class]])
        nativeWindow = static_cast<NSWindow *>(nativeView);

    if (nativeWindow == nil)
        return;
    
    nativeWindow.titlebarAppearsTransparent = true;
    QColor background = widget->palette().color(QPalette::Window);
    nativeWindow.backgroundColor = [NSColor colorWithRed:background.redF()
                                                   green:background.greenF()
                                                    blue:background.blueF()
                                                   alpha:1];
    widget->setProperty("QXMacTitleBarHidden", true);
}

