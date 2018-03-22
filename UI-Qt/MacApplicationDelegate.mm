/*
  Permission for the use of this code is granted only for research, educational, and non-commercial purposes.

  Redistribution of this code or its parts in source, binary, and any other form without permission, with or without modification, is prohibited.  Modifications include, but are not limited to, translation to other programming languages and reuse of tables, constant definitions, and API's defined in it.

  Andrew Choi is not liable for any losses or damages caused by the use of this code.

  Copyright 2009 Andrew Choi.
*/

#include <QtGui>
#include <QtDebug>

#import "ApplicationDelegate.h"

#include "qudocumentwindowmanager.h"

@implementation ApplicationDelegate

- (id)init
{
  self = [super init];

  if (self)
  {
      terminationPending = NO;
  }

  return self;
}

- (NSApplicationTerminateReply)applicationShouldTerminate:(NSApplication *)sender
{
    Q_UNUSED(sender);

    terminationPending = YES;

    QUDocumentWindowManager::instance()->closeDocumentsAndQuit();

    return NSTerminateLater;
}

- (void)confirmPendingTermination
{
    if (terminationPending)
        [NSApp replyToApplicationShouldTerminate:YES];
}

- (void)cancelTermination
{
    if (terminationPending)
    {
        [NSApp replyToApplicationShouldTerminate:NO];

        terminationPending = NO;
    }
}

@end
