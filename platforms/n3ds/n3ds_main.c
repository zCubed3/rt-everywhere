//
// Copyright (c) 2023 Liam R. (zCubed3)
//

#include <3ds.h>

#include <stdio.h>
#include <stdint.h>

#include <rt_everywhere.h>

#define THREAD_COUNT 2
#define THREAD_STACK_SIZE (4 * 1024)

uint16_t width;
uint16_t height;

volatile int kill_threads = 0;
volatile uint8_t *framebuffer;
volatile uint8_t *framebuffer_alt;

float eye_separation = 0.01F;

Thread threads[THREAD_COUNT];
rte_camera_t camera;
int tonemap = 0;

typedef struct render_rect {
    int x;
    int y;
    int width;
    int height;
} render_rect_t;

void render(render_rect_t rect, const trace_t trace, int eye) {
	volatile uint8_t* target = framebuffer;
	trace_t local_trace = trace;

	if (eye == 1)
		target = framebuffer_alt;

	// Our rendering goes in two passes, we skip half the pixels the first time around
	for (int y = rect.y; y < rect.y + rect.height; y++) {
		if (y % 2 == 0)
			continue;

		int column = y * width * 3;
		for (int x = rect.x; x < rect.x + rect.width; x++) {
			if (kill_threads == 1) {
				return;
			}

			int offset = column + (x * 3);

			local_trace.point.x = y;
			local_trace.point.y = x;

			rvec3_t color;
			trace_pixel(RVEC_OUT(color), local_trace);

			target[offset] = color[2] * 0xFF;
			target[offset + 1] = color[1] * 0xFF;
			target[offset + 2] = color[0] * 0xFF;
		}
	}

	for (int y = rect.y; y < rect.y + rect.height; y++) {
		if (y % 2 == 1)
			continue;

		int column = y * width * 3;
		for (int x = rect.x; x < rect.x + rect.width; x++) {
			if (kill_threads == 1) {
				return;
			}

			int offset = column + (x * 3);

			local_trace.point.x = y;
			local_trace.point.y = x;

			rvec3_t color;
			trace_pixel(RVEC_OUT(color), local_trace);

			target[offset] = color[2] * 0xFF;
			target[offset + 1] = color[1] * 0xFF;
			target[offset + 2] = color[0] * 0xFF;
		}
	}
}

void thread_main(void* arg) {
    int num = (int)arg;

#ifdef CHUNKED_RENDERING
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
#else
	// Slightly offset this camera based on which eye it is
	int eye = num > 0;
	float stereo_separation = eye_separation;

	if (eye == 0)
		stereo_separation *= -1;

	rvec4_t right = {1, 0, 0, 0};
	rvec4_t rel_right;

	rmat4_mul_rvec4(RVEC_OUT(rel_right), camera.mat_v, right);

	rvec3_t right_vec;

	rvec3_copy_rvec4(RVEC_OUT(right_vec), rel_right);
	rvec3_normalize(RVEC_OUT(right_vec));
	rvec3_mul_scalar(RVEC_OUT(right_vec), right_vec, stereo_separation);

	rvec3_t position;
	rvec3_add(RVEC_OUT(position), camera.position, right_vec);

	rte_camera_t eye_camera = rte_setup_camera(camera.viewport, position, camera.rotation);

	render_rect_t slice;

	slice.x = 0;
	slice.y = 0;
	slice.width = width;
	//slice.height = 1;
	slice.height = height;

	trace_t trace;

	trace.camera = eye_camera;
	trace.scene = rte_default_scene();
	trace.tonemapping = tonemap;

	int slices = height / THREAD_COUNT;
	slices = 1;

	for (int s = 0; s < slices; s++)
	{
		render(slice, trace, eye);
		slice.y += THREAD_COUNT;
	}
#endif
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

		int cpu = 0;

		if (i % 2 == 1)
			cpu = 2;

        threads[i] = threadCreate(thread_main, arg, THREAD_STACK_SIZE, priority + 1, cpu, false);
    }
}

void print_console() {
	consoleClear();

	printf("Welcome to RT Everywhere (3DS Port)!\n\n");

	printf("NOTE: The 3DS may become unresponsive!\n");
	printf("NOTE: New 3DS models will be faster!\n");

	printf("\nCONTROLS:\n");
	printf("SELECT = Toggle MSAA\n");
	printf("START = Exit\n");
	printf("A = Toggle Tonemapping\n");

	printf("\nINFO:\n");
	printf("POS: %f, %f, %f\n", camera.position[0], camera.position[1], camera.position[2]);
	printf("EYE SEP: %f\n", eye_separation);
}

int main(int argc, char** argv) {
    gfxInitDefault();
	gfxSet3D(true);

    consoleInit(GFX_BOTTOM, NULL);

    gfxSetDoubleBuffering(GFX_TOP, false);

    // For some reason the 3DS framebuffer is rotated
    // I have no clue why this is!
    // NOTE: The 3DS Framebuffer is BGR
    framebuffer = gfxGetFramebuffer(GFX_TOP, GFX_LEFT, &width, &height);
	framebuffer_alt = gfxGetFramebuffer(GFX_TOP, GFX_RIGHT, &width, &height);

    rte_viewport_t viewport = {height, width};
    camera = rte_default_camera(viewport);

	int initial = 1;
	int last3d = gfxIs3D();

    while (aptMainLoop()) {
        hidScanInput();

        uint32_t key_mask = hidKeysDown();

        if (key_mask & KEY_START) {
            break;
        }

        int dirty_image = initial;

		int w_down = 0;
		int s_down = 0;
		int a_down = 0;
		int d_down = 0;
		int q_down = 0;
		int e_down = 0;

        if (key_mask & KEY_SELECT) {
            if (camera.samples == CAMERA_SAMPLES_ONE) {
                camera.samples = CAMERA_SAMPLES_FOUR;
            } else {
                camera.samples = CAMERA_SAMPLES_ONE;
            }

            dirty_image = 1;
        }

		if (key_mask & KEY_A) {
			tonemap = !tonemap;
			dirty_image = 1;
		}

		if (key_mask & KEY_X) {
			eye_separation += 0.01F;
			dirty_image = 1;
		}

		if (key_mask & KEY_Y) {
			eye_separation -= 0.01F;
			dirty_image = 1;
		}

		a_down = (key_mask & KEY_DLEFT) != 0;
		d_down = (key_mask & KEY_DRIGHT) != 0;
		s_down = (key_mask & KEY_DDOWN) != 0;
		w_down = (key_mask & KEY_DUP) != 0;

		if ((w_down || s_down || a_down || d_down || q_down || e_down)) {
			real_t move_x = 0;
			real_t move_y = 0;
			real_t move_z = 0;

			if (w_down) {
				move_z -= 1;
			}

			if (s_down) {
				move_z += 1;
			}


			if (a_down) {
				move_x -= 1;
			}

			if (d_down) {
				move_x += 1;
			}


			if (q_down) {
				move_y -= 1;
			}

			if (e_down) {
				move_y += 1;
			}

			real_t delta_time = 0.1F;
			real_t gear = REAL(1.0);

			move_x *= delta_time * gear;
			move_y *= delta_time * gear;
			move_z *= delta_time * gear;

			// Calculate the right and forward directions of the current rotation
			rmat4_t rot_matrix;
			rmat4_t rot_matrix_t;

			rvec3_t position;
			rvec3_t rotation;

			rvec3_copy(RVEC_OUT(position), camera.position);
			rvec3_copy(RVEC_OUT(rotation), camera.rotation);

			rmat4_rotate(rot_matrix, rotation);
			rmat4_transpose(rot_matrix_t, rot_matrix);
			rmat4_copy(rot_matrix, rot_matrix_t);

			rvec4_t right = {1, 0, 0, 0};
			rvec4_t up = {0, 1, 0, 0};
			rvec4_t forward = {0, 0, 1, 0};

			rvec4_t rel_right;
			rvec4_t rel_up;
			rvec4_t rel_forward;

			rmat4_mul_rvec4(RVEC_OUT(rel_right), rot_matrix, right);
			rmat4_mul_rvec4(RVEC_OUT(rel_up), rot_matrix, up);
			rmat4_mul_rvec4(RVEC_OUT(rel_forward), rot_matrix, forward);

			rvec3_t right_vec;
			rvec3_t up_vec;
			rvec3_t forward_vec;

			rvec3_copy_rvec4(RVEC_OUT(right_vec), rel_right);
			rvec3_normalize(RVEC_OUT(right_vec));
			rvec3_mul_scalar(RVEC_OUT(right_vec), right_vec, move_x);

			rvec3_copy_rvec4(RVEC_OUT(up_vec), rel_up);
			rvec3_normalize(RVEC_OUT(up_vec));
			rvec3_mul_scalar(RVEC_OUT(up_vec), up_vec, move_y);

			rvec3_copy_rvec4(RVEC_OUT(forward_vec), rel_forward);
			rvec3_normalize(RVEC_OUT(forward_vec));
			rvec3_mul_scalar(RVEC_OUT(forward_vec), forward_vec, move_z);

			rvec3_add(RVEC_OUT(position), position, right_vec);
			rvec3_add(RVEC_OUT(position), position, up_vec);
			rvec3_add(RVEC_OUT(position), position, forward_vec);

			camera = rte_setup_camera(viewport, position, rotation);
			dirty_image = 1;
		}

		if (dirty_image) {
			kill_threads = 1;
			await_threads();
			kill_threads = 0;

			spawn_threads();

			print_console();

			initial = 0;
		}

        gfxFlushBuffers();
        //gfxSwapBuffers();

        gspWaitForVBlank();
    }

    gfxExit();
    return 0;
}