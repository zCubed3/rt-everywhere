//
// Copyright (c) 2023-2025 Liam R. (zCubed3)
//

#import "View.h"

#import <rt_everywhere.h>

@implementation View

- (void)drawRect:(NSRect)dirtyRect {
	viewport_t viewport = {dirtyRect.size.width, dirtyRect.size.height};
	camera_t camera = default_camera(viewport);

	// TODO: Find a MUCH better method of doing this?
	for (int x = dirtyRect.origin.x; x < dirtyRect.size.width; x++) {
		for (int y = dirtyRect.origin.y; y < dirtyRect.size.height; y++) {
			point_t point = {x, y};

			rvec3_t col;
			trace_pixel(col, camera, point);

			NSColor *our_color = [NSColor colorWithRed:col[0] green:col[1] blue:col[2] alpha:1.0];
			[our_color setFill];

			NSRectFill(NSMakeRect(x, y, 1, 1));


		}
	}
}

@end
