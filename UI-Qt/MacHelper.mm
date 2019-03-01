#import <AppKit/AppKit.h>
#import <Cocoa/Cocoa.h>

#include <QVariant>
#include <QWidget>
#include <QWindow>

#include "MacHelper.h"
#include "QXDocumentWindowManager.h"

@interface MacApplicationDelegate : NSObject<NSApplicationDelegate>
- (NSApplicationTerminateReply)applicationShouldTerminate:(NSApplication *)sender;
- (BOOL)applicationShouldOpenUntitledFile:(NSApplication *)sender;
- (BOOL)applicationOpenUntitledFile:(NSApplication *)sender;
@end

@implementation MacApplicationDelegate
- (NSApplicationTerminateReply)applicationShouldTerminate:(NSApplication *)sender {
    Q_UNUSED(sender);
    QXDocumentWindowManager::instance()->closeAllDocumentsAndQuit();
    return NSTerminateNow;
}

- (BOOL)applicationShouldOpenUntitledFile:(NSApplication *)sender {
    return YES;
}

- (BOOL)applicationOpenUntitledFile:(NSApplication *)sender {
    QXDocumentWindowManager::instance()->autoOpenFontDialog();
    return YES;
}

@end


void
MacHelper::hideTitleBar(QWidget * widget) {
    return;
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

void
MacHelper::installNSApplicationDelegate() {
    // Initiate shared application and setup delegate so QApplication constructor (ancestor of QtDocBasedApplication) will pick that up as "reflection delegate" (see src/gui/kernel/qapplication_mac.mm)
    NSAutoreleasePool * pool = [[NSAutoreleasePool alloc] init];
    [[NSApplication sharedApplication] setDelegate:[[MacApplicationDelegate alloc] init]];
    [pool release];
}
