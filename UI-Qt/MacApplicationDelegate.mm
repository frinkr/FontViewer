
#import "MacApplicationDelegate.h"
#include "QUDocumentWindowManager.h"

@implementation MacApplicationDelegate

- (NSApplicationTerminateReply)applicationShouldTerminate:(NSApplication *)sender
{
    Q_UNUSED(sender);


    QUDocumentWindowManager::instance()->closeAllDocumentsAndQuit();

    return NSTerminateNow;
}

@end
