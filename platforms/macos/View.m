/****************************************************************************************/
/* Copyright (c) 2023 zCubed3                                                        */
/*                                                                                      */
/* Permission is hereby granted, free of charge, to any person obtaining a copy         */
/* of this software and associated documentation files (the "Software"), to deal        */
/* in the Software without restriction, including without limitation the rights         */
/* to use, copy, modify, merge, publish, distribute, sublicense, and/or sell            */
/* copies of the Software, and to permit persons to whom the Software is                */
/* furnished to do so, subject to the following conditions:                             */
/*                                                                                      */
/* The above copyright notice and this permission notice shall be included in all       */
/* copies or substantial portions of the Software.                                      */
/*                                                                                      */
/* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR           */
/* IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,             */
/* FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE          */
/* AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER               */
/* LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,        */
/* OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE        */
/* SOFTWARE.                                                                            */
/****************************************************************************************/

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
