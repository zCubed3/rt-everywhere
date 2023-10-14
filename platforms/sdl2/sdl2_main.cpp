//
// Copyright (c) 2023 Liam R. (zCubed3)
//

#include <cstdlib>
#include <cstdio>

#include <SDL.h>

#include <rt_everywhere.h>

extern "C" {
    #include <image/bmp.h>
};

#ifdef RTEVERYWHERE_IMGUI
#include <imgui.h>
#include <backends/imgui_impl_sdl2.h>
#include <backends/imgui_impl_sdlrenderer2.h>
#endif

#define PREVIEW_SIZE 128

int pixels_rendered = 0;
int pixel_count = 1;
int thread_alive = 1;
int should_render = 0;

uint32_t time_render_start;
uint32_t time_render_end;

int use_msaa = 0;

rvec3_t position;
rvec3_t rotation;

SDL_Rect texture_rect;
SDL_Rect preview_rect;

SDL_Texture* texture = NULL;
SDL_Texture* preview_texture = NULL;

SDL_Thread* render_thread = NULL;

camera_t camera;

void get_rgb(rvec3_out_t dst, const uint8_t* src, int x, int y, int width, int stride) {
    int index = (y * width * stride) + (x * stride);

    dst[0] = src[index] / 255.0;
    dst[1] = src[index + 1] / 255.0;
    dst[2] = src[index + 2] / 255.0;
}

int render(int is_preview) {
	uint8_t *pixels = NULL;
	int pitch;

	SDL_Texture *target_texture = texture;
	SDL_Rect target_rect = texture_rect;

	if (is_preview) {
		target_texture = preview_texture;
		target_rect = preview_rect;
	}

	if (target_texture == NULL) {
		printf("Error: Target texture was NULL!\n");
		return 1;
	}

	SDL_LockTexture(target_texture, NULL, (void **) &pixels, &pitch);

	pixel_count = target_rect.w * target_rect.h;
	pixels_rendered = 0;

	viewport_t viewport;
	viewport.width = target_rect.w;
	viewport.height = target_rect.h;

	camera = setup_camera(viewport, position, rotation);

	if (!use_msaa || is_preview) {
		camera.samples = CAMERA_SAMPLES_ONE;
	} else {
		camera.samples = CAMERA_SAMPLES_FOUR;
	}

	for (int y = 0; y < target_rect.h; y++) {
		for (int x = 0; x < target_rect.w; x++) {
			int index = (y * target_rect.w * 4) + (x * 4);

			rvec3_t color;
			point_t point;
			point.x = x;
			point.y = y;

			trace_pixel(RVEC_OUT(color), camera, point);

			pixels[index + 2] = color[0] * 255;
			pixels[index + 1] = color[1] * 255;
			pixels[index + 0] = color[2] * 255;

			pixels_rendered++;
		}
	}

	SDL_UnlockTexture(target_texture);

    // We have to rewrite our pixel data for BMPs
    uint8_t* fix_pixels = new uint8_t[target_rect.w * target_rect.h * 3];
    for (int y = 0; y < target_rect.h; y++) {
        int inv_y = target_rect.h - 1 - y;

        for (int x = 0; x < target_rect.w; x++) {
            int old_index = (y * target_rect.w * 4) + (x * 4);
            int fix_index = (inv_y * target_rect.w * 3) + (x * 3);

            fix_pixels[fix_index + 2] = pixels[old_index + 2];
            fix_pixels[fix_index + 1] = pixels[old_index + 1];
            fix_pixels[fix_index + 0] = pixels[old_index + 0];
        }
    }

    write_bmp("out.bmp", texture_rect.w, texture_rect.h, (char*)fix_pixels);

    delete[] fix_pixels;

	return 0;
}

int render_loop(void* data) {
    time_render_start = SDL_GetTicks();
	int state = render(0);
    time_render_end = SDL_GetTicks();

	should_render = 0;
	render_thread = NULL;

	return state;
}

int main(int argc, char** argv) {
    SDL_Init(SDL_INIT_EVERYTHING);

    SDL_Window* window = SDL_CreateWindow(
        "RT Everywhere (SDL2)",
        SDL_WINDOWPOS_CENTERED,
        SDL_WINDOWPOS_CENTERED,
        800,
        600,
        SDL_WINDOW_RESIZABLE
    );

    if (window == NULL) {
        printf("FATAL: SDL Window was NULL!\n");
        return 1;
    }

    int sdl_renderer_flags = SDL_RENDERER_PRESENTVSYNC;

#ifdef __APPLE__
    sdl_renderer_flags |= SDL_RENDERER_SOFTWARE;
#else
    sdl_renderer_flags |= SDL_RENDERER_ACCELERATED;
#endif

    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, sdl_renderer_flags);

    if (renderer == NULL) {
        printf("FATAL: SDL Renderer was NULL!\n");
        return 1;
    }

    // Preview texture
    preview_texture = SDL_CreateTexture(
        renderer,
        SDL_PIXELFORMAT_ARGB8888,
        SDL_TEXTUREACCESS_STREAMING,
        PREVIEW_SIZE,
        PREVIEW_SIZE
    );

    preview_rect.x = 0;
    preview_rect.y = 0;
    preview_rect.w = PREVIEW_SIZE;
    preview_rect.h = PREVIEW_SIZE;

    // Create a temporary camera to get the default values
    if (1) {
        camera_t temp = default_camera({64, 64});
        rvec3_copy(RVEC_OUT(position), temp.position);
        rvec3_copy(RVEC_OUT(rotation), temp.rotation);
    }

#ifdef RTEVERYWHERE_IMGUI
    ImGuiContext* imgui_context = ImGui::CreateContext();
    ImGui::SetCurrentContext(imgui_context);

    ImGuiIO& io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;

    ImGui_ImplSDL2_InitForSDLRenderer(window, renderer);
    ImGui_ImplSDLRenderer2_Init(renderer);
#endif

    SDL_Event event;
    int run = 1;

    int lock_cursor = 0;
    int recreate_texture = 1;

    uint32_t last;
    uint32_t now = SDL_GetTicks();

    int manual_width = 320;
    int manual_height = 240;
    int manual_size = 0;

    int w_down = 0;
    int s_down = 0;
    int a_down = 0;
    int d_down = 0;

    while (run) {
        int draw_preview = 0;

        //https://gamedev.stackexchange.com/questions/110825/how-to-calculate-delta-time-with-sdl
        last = now;
        now = SDL_GetPerformanceCounter();
        float delta_time = (float) (now - last) / (float) SDL_GetPerformanceFrequency();

        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                run = 0;
                break;
            }

            if (event.type == SDL_WINDOWEVENT) {
                if (event.window.event == SDL_WINDOWEVENT_RESIZED) {
                    recreate_texture = 1;
                }
            }

            if (event.type == SDL_KEYDOWN) {
                if (event.key.keysym.scancode == SDL_SCANCODE_W) {
                    w_down = 1;
                }

                if (event.key.keysym.scancode == SDL_SCANCODE_S) {
                    s_down = 1;
                }

                if (event.key.keysym.scancode == SDL_SCANCODE_A) {
                    a_down = 1;
                }

                if (event.key.keysym.scancode == SDL_SCANCODE_D) {
                    d_down = 1;
                }
            }

            if (event.type == SDL_KEYUP) {
                if (event.key.keysym.scancode == SDL_SCANCODE_W) {
                    w_down = 0;
                }

                if (event.key.keysym.scancode == SDL_SCANCODE_S) {
                    s_down = 0;
                }

                if (event.key.keysym.scancode == SDL_SCANCODE_A) {
                    a_down = 0;
                }

                if (event.key.keysym.scancode == SDL_SCANCODE_D) {
                    d_down = 0;
                }
            }

            if (event.type == SDL_MOUSEMOTION) {
                if (lock_cursor) {
                    rotation[1] += event.motion.xrel / 10.0F;
                    rotation[0] += event.motion.yrel / 10.0F;
                }
            }

            if (event.type == SDL_MOUSEBUTTONDOWN) {
                if (event.button.button == SDL_BUTTON_RIGHT) {
                    lock_cursor = !lock_cursor;
                    SDL_SetRelativeMouseMode(static_cast<SDL_bool>(lock_cursor));

                    if (lock_cursor == 0) {
                        should_render = 1;
                    }
                }
            }

#ifdef RTEVERYWHERE_IMGUI
            ImGui_ImplSDL2_ProcessEvent(&event);
#endif
        }

        if (lock_cursor) {
            draw_preview = 1;
        }

        if ((w_down || s_down || a_down || d_down) && lock_cursor) {
            float move_x = 0;
            float move_z = 0;

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

            move_x *= delta_time;
            move_z *= delta_time;

            // Calculate the right and forward directions of the current rotation
            rmat4_t rot_matrix;
            rmat4_t rot_matrix_t;

            rmat4_rotate(rot_matrix, rotation);
            rmat4_transpose(rot_matrix_t, rot_matrix);
            rmat4_copy(rot_matrix, rot_matrix_t);

            rvec4_t right = {1, 0, 0, 0};
            rvec4_t forward = {0, 0, 1, 0};

            rvec4_t rel_right;
            rvec4_t rel_forward;

            rmat4_mul_rvec4(RVEC_OUT(rel_right), rot_matrix, right);
            rmat4_mul_rvec4(RVEC_OUT(rel_forward), rot_matrix, forward);

            rvec3_t right_vec;
            rvec3_t forward_vec;

            rvec3_copy_rvec4(RVEC_OUT(right_vec), rel_right);
            rvec3_normalize(RVEC_OUT(right_vec));
            rvec3_mul_scalar(RVEC_OUT(right_vec), right_vec, move_x);

            rvec3_copy_rvec4(RVEC_OUT(forward_vec), rel_forward);
            rvec3_normalize(RVEC_OUT(forward_vec));
            rvec3_mul_scalar(RVEC_OUT(forward_vec), forward_vec, move_z);

            rvec3_add(RVEC_OUT(position), position, right_vec);
            rvec3_add(RVEC_OUT(position), position, forward_vec);
        }

        if (draw_preview && !should_render && render_thread == NULL) {
            render(1);
        }

        if (should_render && render_thread == NULL) {
            render_thread = SDL_CreateThread(render_loop, "RTEverywhereRenderLoop", NULL);
        }

        if (recreate_texture && !should_render) {
            if (render_thread != NULL) {
                int status;
                SDL_WaitThread(render_thread, &status);
            }

            SDL_DestroyTexture(texture);
            texture = NULL;

            int width;
            int height;
            SDL_GetWindowSizeInPixels(window, &width, &height);

            if (manual_size) {
                width = manual_width;
                height = manual_height;
            }

            texture = SDL_CreateTexture(
                renderer,
                SDL_PIXELFORMAT_ARGB8888,
                SDL_TEXTUREACCESS_STREAMING,
                width,
                height
            );

            texture_rect.x = 0;
            texture_rect.y = 0;
            texture_rect.w = width;
            texture_rect.h = height;

            recreate_texture = 0;
            should_render = 1;
        }

#ifdef RTEVERYWHERE_IMGUI
        ImGui_ImplSDL2_NewFrame(window);
        ImGui_ImplSDLRenderer2_NewFrame();
        ImGui::NewFrame();
#endif

#ifdef RTEVERYWHERE_IMGUI
        ImGui::Begin("Render");

        if (ImGui::CollapsingHeader("Render")) {
            float frac = (float) pixels_rendered / (float) pixel_count;
            ImGui::Text("Rendered: %i out of %i pixels", pixels_rendered, pixel_count);
            ImGui::Text("Resolution: %i by %i", texture_rect.w, texture_rect.h);

            ImGui::ProgressBar(frac);

            if (time_render_start <= time_render_end) {
                uint32_t elapsed = time_render_end - time_render_start;
                ImGui::Text("Last render took %ums (%fs)", elapsed, (float)elapsed / 1000.0F);
            }
        }

        if (ImGui::CollapsingHeader("State")) {
            ImGui::Checkbox("MSAA?", reinterpret_cast<bool*>(&use_msaa));

            ImGui::InputInt("Width", &manual_width);
            ImGui::InputInt("Height", &manual_height);

            ImGui::Checkbox("Manual Size?", (bool*)&manual_size);

            if (ImGui::Button("Recreate image")) {
                recreate_texture = 1;
            }

            if (ImGui::Button("Re-render")) {
                should_render = 1;
            }
        }

        ImGui::End();
#endif

        SDL_Texture* target_texture = texture;
        SDL_Rect target_rect = texture_rect;

        if (draw_preview) {
            target_texture = preview_texture;
            target_rect = preview_rect;
        }

        SDL_RenderClear(renderer);

        //SDL_RenderCopy(renderer, texture, NULL, &texture_rect);
        SDL_RenderCopy(renderer, target_texture, NULL, NULL);

#ifdef RTEVERYWHERE_IMGUI
        ImGui::Render();
        ImGui_ImplSDLRenderer2_RenderDrawData(ImGui::GetDrawData());
#endif

        SDL_RenderPresent(renderer);
    }

    if (render_thread != NULL) {
        int state;
        SDL_WaitThread(render_thread, &state);
    }

    return 0;
}