#include <QWidget>

#import "MacHelper.h"
#import <AppKit/AppKit.h>

void
MacHelper::hideTitleBar(QWidget * widget)
{
    NSView *nativeView = reinterpret_cast<NSView *>(widget->winId());
    NSWindow* nativeWindow = nativeView.window;

    // Default masks
    nativeWindow.styleMask = NSWindowStyleMaskTitled | NSWindowStyleMaskClosable | NSWindowStyleMaskMiniaturizable | NSWindowStyleMaskResizable;

    // Content under title bar
    nativeWindow.styleMask |= NSWindowStyleMaskFullSizeContentView;
//    nativeWindow.styleMask |= NSWindowStyleMaskHUDWindow;
    
    nativeWindow.titlebarAppearsTransparent = true;
    //nativeWindow.titleVisibility = NSWindowTitleHidden; // Hide window title name
}
