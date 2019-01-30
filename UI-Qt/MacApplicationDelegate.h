
#import <Cocoa/Cocoa.h>

@interface MacApplicationDelegate : NSObject<NSApplicationDelegate>
- (NSApplicationTerminateReply)applicationShouldTerminate:(NSApplication *)sender;
- (BOOL)applicationShouldOpenUntitledFile:(NSApplication *)sender;
@end
