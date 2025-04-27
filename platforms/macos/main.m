//
// Copyright (c) 2023-2025 Liam R. (zCubed3)
//

#import <Cocoa/Cocoa.h>

#import "AppDelegate.h"

// Referenced: https://github.com/rgl/minimal-cocoa-app/blob/master/main.m
int main(int argc, const char * argv[]) {
	@autoreleasepool {
		AppDelegate *app_delegate = [AppDelegate alloc];

		[NSApplication sharedApplication];

		[NSApp setActivationPolicy:NSApplicationActivationPolicyRegular];

		dispatch_async(dispatch_get_main_queue(), ^{
			[NSApp activateIgnoringOtherApps:YES];
		});

		[NSApp setDelegate:app_delegate];
		[NSApp run];
	}

	return 0;
}
