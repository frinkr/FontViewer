
#import "MacApplicationDelegate.h"
#include "QXDocumentWindowManager.h"
#include "QXFontManager.h"

@implementation MacApplicationDelegate

- (void)applicationDidFinishLaunching:(NSNotification *)notification {
    Q_UNUSED(notification);
    if (QXFontManager::checkResetDatabase())
        QXFontManager::resetDatabase();    
}

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
