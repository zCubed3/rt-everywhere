//
// Copyright (c) 2023-2025 Liam R. (zCubed3)
//

#import "AppDelegate.h"
#import "View.h"

#import <Cocoa/Cocoa.h>

@interface AppDelegate ()


@end

@implementation AppDelegate

NSWindow *window = NULL;

- (void)applicationDidFinishLaunching:(NSNotification *)aNotification {
    // Insert code here to initialize your application
    
    // Create our window
	window = [[NSWindow alloc] autorelease];

	NSRect rect = NSMakeRect(0, 0, 256, 256);
	[window initWithContentRect:rect
			styleMask:NSTitledWindowMask | NSWindowStyleMaskClosable | NSWindowStyleMaskResizable
			backing:NSBackingStoreBuffered
			defer:NO
	];

	[window setTitle:@"RT Everywhere"];
	[window center];
	[window makeKeyAndOrderFront:nil];
	[window display];

	View *view = [[View alloc] autorelease];
	[view initWithFrame:rect];

	window.contentView = view;
}


- (void)applicationWillTerminate:(NSNotification *)aNotification {
    // Insert code here to tear down your application

	printf("Goodbye!\n");
}


- (BOOL)applicationSupportsSecureRestorableState:(NSApplication *)app {
    return YES;
}


@end
