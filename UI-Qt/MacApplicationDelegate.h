#import <Cocoa/Cocoa.h>

@interface MacApplicationDelegate : NSObject<NSApplicationDelegate>
- (void)applicationDidFinishLaunching:(NSNotification *)notification;
- (NSApplicationTerminateReply)applicationShouldTerminate:(NSApplication *)sender;
- (BOOL)applicationShouldOpenUntitledFile:(NSApplication *)sender;
@end
