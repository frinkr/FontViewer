#include <QWidget>

#import <AppKit/AppKit.h>
#import "MacApplicationDelegate.h"
#import "MacHelper.h"


void
MacHelper::hideTitleBar(QWidget * widget)
{
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
                                                    blue: background.blueF()
                                                   alpha: 1];
}


void
MacHelper::installNSApplicationDelegate() {
    NSAutoreleasePool * pool = [[NSAutoreleasePool alloc] init];
    [[NSApplication sharedApplication] setDelegate:[[MacApplicationDelegate alloc] init]];
    [pool release];
}
