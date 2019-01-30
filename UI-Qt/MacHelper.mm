#include <QWidget>

#import <AppKit/AppKit.h>
#import "MacApplicationDelegate.h"
#import "MacHelper.h"


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


void
MacHelper::installNSApplicationDelegate() {
    NSAutoreleasePool * pool = [[NSAutoreleasePool alloc] init];
    [[NSApplication sharedApplication] setDelegate:[[MacApplicationDelegate alloc] init]];
    [pool release];
}
