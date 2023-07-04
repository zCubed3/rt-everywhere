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

#define THREAD_COUNT 4
#define THREAD_STACK_SIZE (4 * 1024)

uint16_t width;
uint16_t height;

volatile int kill_threads = 0;
volatile uint8_t *framebuffer;

Thread threads[THREAD_COUNT];
camera_t camera;

typedef struct render_rect {
	int x;
	int y;
	int width;
	int height;
} render_rect_t;

void render(render_rect_t rect) {
	for (int y = rect.y; y < rect.y + rect.height; y++) {
		int column = y * width * 3;
		for (int x = rect.x; x < rect.x + rect.width; x++) {
			if (kill_threads == 1) {
				return;
			}

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

void thread_main(void* arg) {
	int num = (int)arg;

	// We need to divide our framebuffer into equal sections
	int per_x = width / 2;
	int per_y = height / 2;

	render_rect_t rect;

	if (num == 1 || num == 3) {
		rect.x = per_x;
	} else {
		rect.x = 0;
	}

	if (num == 1 || num == 2) {
		rect.y = per_y;
	} else {
		rect.y = 0;
	}

	rect.width = per_x;
	rect.height = per_y;

	render(rect);
}

void await_threads() {
	for (int t = 0; t < THREAD_COUNT; t++) {
		threadJoin(threads[t], U64_MAX);
		threadFree(threads[t]);
	}
}

void spawn_threads() {
	int32_t priority = 0;
	svcGetThreadPriority(&priority, CUR_THREAD_HANDLE);

	for (int i = 0; i < THREAD_COUNT; i++) {
		void* arg = (void*)i;
		threads[i] = threadCreate(thread_main, arg, THREAD_STACK_SIZE, priority + 1, -2, false);
	}
}

int main(int argc, char** argv) {
	gfxInitDefault();

	consoleInit(GFX_BOTTOM, NULL);

	gfxSetDoubleBuffering(GFX_TOP, false);

	printf("Welcome to RT Everywhere (3DS Port)!\n\n");

	printf("NOTE: The 3DS may become unresponsive!\n");
	printf("NOTE: New 3DS models will be faster!\n");

	// For some reason the 3DS framebuffer is rotated
	// I have no clue why this is!
	// NOTE: The 3DS Framebuffer is BGR
	framebuffer = gfxGetFramebuffer(GFX_TOP, GFX_LEFT, &width, &height);

	viewport_t viewport = {height, width};
	camera = default_camera(viewport);

	printf("\nCONTROLS:\n");
	printf("SELECT = Toggle MSAA\n");
	printf("START = Exit\n");

	spawn_threads();

	while (aptMainLoop()) {
		hidScanInput();

		uint32_t key_mask = hidKeysDown();

		if (key_mask & KEY_START) {
			break;
		}

		int dirty_image = 0;

		if (key_mask & KEY_SELECT) {
			if (camera.samples == CAMERA_SAMPLES_ONE) {
				camera.samples = CAMERA_SAMPLES_FOUR;
			} else {
				camera.samples = CAMERA_SAMPLES_ONE;
			}

			dirty_image = 1;
		}

		if (dirty_image) {
			kill_threads = 1;
			await_threads();
			kill_threads = 0;

			spawn_threads();
		}

		gfxFlushBuffers();
		//gfxSwapBuffers();

		gspWaitForVBlank();
	}

	gfxExit();
	return 0;
}