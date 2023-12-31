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

#define PREVIEW_SIZE_X 228
#define PREVIEW_SIZE_Y 128

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

SDL_Texture* render_texture = NULL;
SDL_Rect render_rect;
uint8_t* render_pixels = NULL;
rte_bool_e render_lock = RTE_FALSE;
int render_semaphore = 0;

SDL_Thread** sdl_threads = NULL;
int sdl_concurrency;
int actual_concurrency = 0;
int render_concurrency = 0;

rte_camera_t camera;
rte_scene_t scene;
rte_tonemap_e tonemapping = RTE_TONEMAP_NONE;

typedef enum render_target {
    RENDER_TARGET_SCREEN,
    RENDER_TARGET_PREVIEW
} render_target_e;

typedef struct render_thread {
    SDL_Thread** pp_thread;
    SDL_Rect rect;
    int thread_index;
} render_thread_t;

void get_rgb(rvec3_out_t dst, const uint8_t* src, int x, int y, int width, int stride) {
    int index = (y * width * stride) + (x * stride);

    dst[0] = src[index] / 255.0;
    dst[1] = src[index + 1] / 255.0;
    dst[2] = src[index + 2] / 255.0;
}

void begin_render(render_target_e target) {
    int pitch;

    render_texture = texture;
    render_rect = texture_rect;

    if (target == RENDER_TARGET_PREVIEW) {
        render_texture = preview_texture;
        render_rect = preview_rect;
    }

    SDL_LockTexture(render_texture, NULL, (void **) &render_pixels, &pitch);

    pixels_rendered = 0;
    pixel_count = render_rect.w * render_rect.h;
    render_lock = RTE_TRUE;
    time_render_start = SDL_GetTicks();
}

void end_render() {
    time_render_end = SDL_GetTicks();

    uint8_t* fix_pixels = new uint8_t[render_rect.w * render_rect.h * 3];
    for (int y = 0; y < render_rect.h; y++) {
        int inv_y = render_rect.h - 1 - y;

        for (int x = 0; x < render_rect.w; x++) {
            int old_index = (y * render_rect.w * 4) + (x * 4);
            int fix_index = (inv_y * render_rect.w * 3) + (x * 3);

            fix_pixels[fix_index + 2] = render_pixels[old_index + 2];
            fix_pixels[fix_index + 1] = render_pixels[old_index + 1];
            fix_pixels[fix_index + 0] = render_pixels[old_index + 0];
        }
    }

    write_bmp("out.bmp", texture_rect.w, texture_rect.h, (char*)fix_pixels);

    delete[] fix_pixels;

    SDL_UnlockTexture(render_texture);
    render_lock = RTE_FALSE;
}

int render(render_target_e target, SDL_Rect rect) {
	int pitch;

	if (render_texture == NULL) {
		printf("Error: Render texture was NULL!\n");
		return 1;
	}

	rte_viewport_t viewport;
	viewport.width = render_rect.w;
	viewport.height = render_rect.h;

    camera = rte_setup_camera(viewport, position, rotation);

	if (!use_msaa || target == RENDER_TARGET_PREVIEW) {
        camera.samples = CAMERA_SAMPLES_ONE;
	} else {
        camera.samples = CAMERA_SAMPLES_FOUR;
	}

    trace_t trace;

    trace.camera = camera;
    trace.scene = scene;
    trace.tonemapping = tonemapping;

	for (int y = rect.y; y < rect.y + rect.h; y++) {
		for (int x = rect.x; x < rect.x + rect.w; x++) {
			int index = (y * render_rect.w * 4) + (x * 4);

			rvec3_t color;

            trace.point.x = x;
            trace.point.y = y;

			trace_pixel(RVEC_OUT(color), trace);

			render_pixels[index + 2] = color[0] * 255;
			render_pixels[index + 1] = color[1] * 255;
			render_pixels[index + 0] = color[2] * 255;

			pixels_rendered++;
		}
	}

	return 0;
}

int render_loop(void* data) {
    render_thread_t* args = (render_thread_t*)data;

    int status = render(RENDER_TARGET_SCREEN, args->rect);

    *args->pp_thread = NULL;
    render_semaphore--;

    free(args);
	return 0;
}

inline int threads_done() {
    if (actual_concurrency == 0 || sdl_threads == NULL)
        return 1;

    if (sdl_threads != NULL) {
        for (int t = 0; t < actual_concurrency; t++) {
            if (sdl_threads[t] != NULL) {
                return 0; // Not done
            }
        }
    }

    return 1;
}

inline void wait_for_threads() {
    if (sdl_threads != NULL) {
        for (int t = 0; t < actual_concurrency; t++) {
            if (sdl_threads[t] != NULL) {
                int status;
                SDL_WaitThread(sdl_threads[t], &status);
            }
        }
    }
}

int main(int argc, char** argv) {
    SDL_Init(SDL_INIT_EVERYTHING);

    SDL_Window* window = SDL_CreateWindow(
        "RT Everywhere (SDL2)",
        SDL_WINDOWPOS_CENTERED,
        SDL_WINDOWPOS_CENTERED,
        640,
        360,
        SDL_WINDOW_RESIZABLE
    );

    if (window == NULL) {
        printf("FATAL: SDL Window was NULL!\n");
        return 1;
    }

    int sdl_renderer_flags = SDL_RENDERER_PRESENTVSYNC;

#if defined(__APPLE__) || 1
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
        PREVIEW_SIZE_X,
        PREVIEW_SIZE_Y
    );

    preview_rect.x = 0;
    preview_rect.y = 0;
    preview_rect.w = PREVIEW_SIZE_X;
    preview_rect.h = PREVIEW_SIZE_Y;

    sdl_concurrency = SDL_GetCPUCount();
    actual_concurrency = sdl_concurrency;

    // Create a temporary camera to get the default values
    if (1) {
        rte_camera_t temp = rte_default_camera({64, 64});
        rvec3_copy(RVEC_OUT(position), temp.position);
        rvec3_copy(RVEC_OUT(rotation), temp.rotation);
    }

    scene = rte_default_scene();

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
    int q_down = 0;
    int e_down = 0;

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

                if (event.key.keysym.scancode == SDL_SCANCODE_Q) {
                    q_down = 1;
                }

                if (event.key.keysym.scancode == SDL_SCANCODE_E) {
                    e_down = 1;
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

                if (event.key.keysym.scancode == SDL_SCANCODE_Q) {
                    q_down = 0;
                }

                if (event.key.keysym.scancode == SDL_SCANCODE_E) {
                    e_down = 0;
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

        if ((w_down || s_down || a_down || d_down || q_down || e_down) && lock_cursor) {
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

            SDL_Keymod mod = SDL_GetModState();

            real_t gear = REAL(1.0);

            if (mod & (KMOD_LSHIFT | KMOD_RSHIFT))
                gear = REAL(4.0);

            move_x *= delta_time * gear;
            move_y *= delta_time * gear;
            move_z *= delta_time * gear;

            // Calculate the right and forward directions of the current rotation
            rmat4_t rot_matrix;
            rmat4_t rot_matrix_t;

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
        }

        if (draw_preview && !should_render && threads_done()) {
            begin_render(RENDER_TARGET_PREVIEW);
            render(RENDER_TARGET_PREVIEW, render_rect);
            end_render();
        }

        if (should_render && threads_done()) {
            begin_render(RENDER_TARGET_SCREEN);

            render_semaphore = actual_concurrency;

            if (sdl_threads == NULL) {
                sdl_threads = (SDL_Thread**) calloc(sizeof(SDL_Thread*), actual_concurrency);
            }

            int slice = render_rect.w / actual_concurrency;

            SDL_Rect rect = render_rect;
            rect.w = slice;

            render_concurrency = actual_concurrency;

            for (int c = 0; c < actual_concurrency; c++) {
                render_thread_t* thread = (render_thread_t*) malloc(sizeof(render_thread_t));

                rect.x = c * slice;

                thread->pp_thread = &sdl_threads[c];
                thread->rect = rect;
                thread->thread_index = c;

                sdl_threads[c] = SDL_CreateThread(render_loop, "RTEverywhereRenderThread", thread);
            }

            should_render = 0;
        }

        if (render_semaphore == 0 && threads_done() && render_lock) {
            end_render();
        }

        if (recreate_texture && render_semaphore == 0) {
            wait_for_threads();

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

        if (ImGui::CollapsingHeader("Render Status")) {
            float frac = (float) pixels_rendered / (float) pixel_count;
            ImGui::Text("Rendered: %i out of %i pixels", pixels_rendered, pixel_count);
            ImGui::Text("Resolution: %i by %i", texture_rect.w, texture_rect.h);

            ImGui::ProgressBar(frac);

            if (time_render_start <= time_render_end) {
                uint32_t elapsed = time_render_end - time_render_start;
                ImGui::Text("Last render took %ums (%fs)", elapsed, (float)elapsed / 1000.0F);
                ImGui::Text("Last render ran on %i threads", render_concurrency);
            }
        }

        if (ImGui::CollapsingHeader("Render Config")) {
            ImGui::Checkbox("MSAA?", reinterpret_cast<bool*>(&use_msaa));

            ImGui::Checkbox("ACES Tonemap?", reinterpret_cast<bool*>(&tonemapping));

            ImGui::InputInt("Width", &manual_width);
            ImGui::InputInt("Height", &manual_height);

            ImGui::Checkbox("Manual Size?", (bool*)&manual_size);

            ImGui::DragInt("Concurrency (Threads)", &actual_concurrency, 1.0F, 1, sdl_concurrency);

            if (ImGui::Button("Recreate image")) {
                recreate_texture = 1;
            }

            if (ImGui::Button("Re-render")) {
                should_render = 1;
            }
        }

        if (ImGui::CollapsingHeader("Scene")) {
            ImGui::Indent();

            if (ImGui::CollapsingHeader("Sun")) {
                ImGui::Indent();

                ImGui::DragFloat("Intensity", &scene.sun_light.intensity, 1.0F, 0.01F, 1000.0F);

                ImGui::Unindent();
            }

            ImGui::Unindent();
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

    wait_for_threads();

    return 0;
}