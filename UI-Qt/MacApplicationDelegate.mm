
#import "MacApplicationDelegate.h"
#include "QXDocumentWindowManager.h"

@implementation MacApplicationDelegate

- (NSApplicationTerminateReply)applicationShouldTerminate:(NSApplication *)sender {
    Q_UNUSED(sender);

    QXDocumentWindowManager::instance()->closeAllDocumentsAndQuit();

    return NSTerminateNow;
}

- (BOOL)applicationShouldOpenUntitledFile:(NSApplication *)sender {
    Q_UNUSED(sender);
    QXDocumentWindowManager::instance()->doOpenFontDialog();
    return NO;
}
@end
