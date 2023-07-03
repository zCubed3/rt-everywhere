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

#include <3ds.h>

#include <stdio.h>
#include <stdint.h>

#include <rt_everywhere.h>

void render(camera_t camera, uint16_t width, uint16_t height, uint8_t framebuffer) {
	for (uint16_t y = 0; y < height; y++) {
		int column = y * width * 3;
		for (uint16_t x = 0; x < width; x++) {
			int offset = column + (x * 3);

			point_t point = {y, x};

			rvec3_t color;
			trace_pixel(color, camera, point);

			framebuffer[offset] = color[2] * 0xFF;
			framebuffer[offset + 1] = color[1] * 0xFF;
			framebuffer[offset + 2] = color[0] * 0xFF;
		}
	}
}

int main(int argc, char** argv) {
	gfxInitDefault();

	consoleInit(GFX_BOTTOM, NULL);

	uint16_t width;
	uint16_t height;

	gfxSetDoubleBuffering(GFX_TOP, false);
	uint8_t *framebuffer = gfxGetFramebuffer(GFX_TOP, GFX_LEFT, &width, &height);

	printf("Welcome to RT Everywhere!\n");

	printf("NOTE: When rendering the 3DS will become unresponsive!\n");

	// For some reason the 3DS framebuffer is rotated
	// I have no clue why this is!
	// NOTE: The 3DS Framebuffer is BGR
	viewport_t viewport = {height, width};
	camera_t camera = default_camera(viewport);

	render(camera, width, height, framebuffer);

	printf("Press START to exit!\n");

	while (aptMainLoop()) {
		hidScanInput();

		uint32_t key_mask = hidKeysDown();

		if (key_mask & KEY_START) {
			break;
		}

		gfxFlushBuffers();
		//gfxSwapBuffers();

		gspWaitForVBlank();
	}

	gfxExit();
	return 0;
}