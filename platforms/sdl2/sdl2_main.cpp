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

#include <cstdlib>
#include <cstdio>

#include <SDL.h>

#include <rt_everywhere.h>

#ifdef RT_HARNESS_SDL2_IMGUI
#include <imgui.h>
#include <backends/imgui_impl_sdl2.h>
#include <backends/imgui_impl_sdlrenderer2.h>
#endif

int pixels_rendered = 0;
int pixel_count = 1;
int thread_alive = 1;
int should_render = 0;

SDL_Rect texture_rect;
SDL_Texture* texture = NULL;
SDL_Thread* render_thread = NULL;

viewport_t viewport = {256, 256};
camera_t camera;

int render_loop(void* data) {
	uint8_t *pixels = NULL;
	int pitch;

	if (texture == NULL) {
		printf("FATAL: SDL Texture was NULL!\n");
		return 1;
	}

	SDL_LockTexture(texture, NULL, (void **) &pixels, &pitch);

	pixel_count = texture_rect.w * texture_rect.h;
	pixels_rendered = 0;

	for (int y = 0; y < texture_rect.h; y++) {
		for (int x = 0; x < texture_rect.w; x++) {
			int index = (y * texture_rect.w * 4) + (x * 4);

			rvec3_t color;
			point_t point;
			point.x = x;
			point.y = y;

			trace_pixel(color, camera, point);

			pixels[index + 2] = color[0] * 255;
			pixels[index + 1] = color[1] * 255;
			pixels[index + 0] = color[2] * 255;

			pixels_rendered++;
		}
	}

	SDL_UnlockTexture(texture);
	should_render = 0;

	render_thread = NULL;

	return 0;
}

int main(int argc, char** argv) {
	SDL_Init(SDL_INIT_EVERYTHING);

	SDL_Window *window = SDL_CreateWindow(
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

	SDL_Renderer *renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

	if (renderer == NULL) {
		printf("FATAL: SDL Renderer was NULL!\n");
		return 1;
	}

#ifdef RT_HARNESS_SDL2_IMGUI
	ImGuiContext *imgui_context = ImGui::CreateContext();
	ImGui::SetCurrentContext(imgui_context);

	ImGuiIO& io = ImGui::GetIO();
	io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;

	ImGui_ImplSDL2_InitForSDLRenderer(window, renderer);
	ImGui_ImplSDLRenderer2_Init(renderer);
#endif

	camera = default_camera(viewport);

	SDL_Event event;
	int run = 1;

	int recreate_texture = 1;

	while (run) {
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

#ifdef RT_HARNESS_SDL2_IMGUI
			ImGui_ImplSDL2_ProcessEvent(&event);
#endif
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

			viewport.width = width;
			viewport.height = height;

			camera = default_camera(viewport);
			camera.samples = CAMERA_SAMPLES_FOUR;

			recreate_texture = 0;
			should_render = 1;
		}

#ifdef RT_HARNESS_SDL2_IMGUI
		ImGui_ImplSDL2_NewFrame(window);
		ImGui_ImplSDLRenderer2_NewFrame();
		ImGui::NewFrame();
#endif

#ifdef RT_HARNESS_SDL2_IMGUI
		ImGui::Begin("Render");

		float frac = (float)pixels_rendered / (float)pixel_count;
		ImGui::ProgressBar(frac);

		bool msaa_enabled = camera.samples == CAMERA_SAMPLES_FOUR;
		ImGui::Checkbox("MSAA?", &msaa_enabled);

		if (!msaa_enabled) {
			camera.samples = CAMERA_SAMPLES_ONE;
		} else {
			camera.samples = CAMERA_SAMPLES_FOUR;
		}

		if (ImGui::Button("Re-render")) {
			should_render = 1;
		}

		ImGui::End();
#endif

		SDL_RenderClear(renderer);

		//SDL_RenderCopy(renderer, texture, NULL, &texture_rect);
		SDL_RenderCopy(renderer, texture, NULL, NULL);

#ifdef RT_HARNESS_SDL2_IMGUI
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