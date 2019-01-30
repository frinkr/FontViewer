
#import "MacApplicationDelegate.h"
#include "QUDocumentWindowManager.h"

@implementation MacApplicationDelegate

- (NSApplicationTerminateReply)applicationShouldTerminate:(NSApplication *)sender {
    Q_UNUSED(sender);

    QUDocumentWindowManager::instance()->closeAllDocumentsAndQuit();

    return NSTerminateNow;
}

- (BOOL)applicationShouldOpenUntitledFile:(NSApplication *)sender {
    Q_UNUSED(sender);
    QUDocumentWindowManager::instance()->doOpenFontDialog();
    return NO;
}
@end
