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
    QXDocumentWindowManager::instance()->autoShowFontListWindow();
    return YES;
}

@end

void
MacHelper::installNSApplicationDelegate() {
    // Initiate shared application and setup delegate so QApplication constructor (ancestor of QtDocBasedApplication) will pick that up as "reflection delegate" (see src/gui/kernel/qapplication_mac.mm)
    NSAutoreleasePool * pool = [[NSAutoreleasePool alloc] init];
    [[NSApplication sharedApplication] setDelegate:[[MacApplicationDelegate alloc] init]];
    [pool release];
}

void
MacHelper::revealFileInFinder(const QString & path) {
    NSArray * fileURLs = [NSArray arrayWithObjects:[NSURL fileURLWithPath:path.toNSString()], nil];
    [[NSWorkspace sharedWorkspace] activateFileViewerSelectingURLs:fileURLs];
}
