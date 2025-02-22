//
// Copyright (c) 2025 Liam Reese
//

#include <cstdlib>
#include <cstdio>

#include <SDL3/SDL.h>

#include <rt_everywhere.hpp>

#include <pipeline/rte_framebuffer.hpp>

#include <glm/glm.hpp>

#ifdef RTEVERYWHERE_IMGUI
#include <imgui.h>
#include <backends/imgui_impl_sdl3.h>
#include <backends/imgui_impl_sdlrenderer3.h>
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

glm::vec3 position;
glm::vec3 rotation;

SDL_Rect texture_rect;
SDL_Rect preview_rect;

SDL_Texture* texture = NULL;
SDL_Texture* preview_texture = NULL;

SDL_Texture* render_texture = NULL;
SDL_Rect render_rect;
uint8_t* render_pixels = NULL;
bool render_lock = false;
int render_semaphore = 0;

SDL_Thread** sdl_threads = NULL;
int sdl_concurrency;
int actual_concurrency = 0;
int render_concurrency = 0;

bool credits_open = true;

rteCamera camera;

class rteSDLFramebuffer : public rteFramebuffer {

protected:
    SDL_Texture* sdlTexture = nullptr;
    int width, height;

    uint8_t* pPixels = nullptr;

    // Only supports ARGB textures!

public:

    rteSDLFramebuffer(SDL_Texture* sdlTexture) {
        this->sdlTexture = sdlTexture;

        float rawX, rawY;
        SDL_GetTextureSize(sdlTexture, &rawX, &rawY);

        this->width = (int)rawX;
        this->height = (int)rawY;
    }

    bool IsThreadSafe() const override {
        return true;
    }

    int GetWidth() const override {
        return width;
    }

    int GetHeight() const override {
        return height;
    }


    void WritePixel(int x, int y, glm::vec4 color) override {

        if (pPixels == nullptr) {
            return; // Pixels aren't locked
        }

        if (x >= width || y >= height) {
            return; // OOB
        }

        int index = (y * width) + x;
        index *= 4;

        pPixels[index + 2] = (uint8_t)(color.r * 255.0F);
        pPixels[index + 1] = (uint8_t)(color.g * 255.0F);
        pPixels[index + 0] = (uint8_t)(color.b * 255.0F);
        pPixels[index + 3] = (uint8_t)(color.a * 255.0F);

    }


    void Lock() override {
        if (sdlTexture == nullptr) {
            return;
        }

        SDL_Rect rect;

        rect.x = 0;
        rect.y = 0;

        rect.w = width;
        rect.h = height;

        int pitch;
        SDL_LockTexture(sdlTexture, &rect, reinterpret_cast<void **>(&pPixels), &pitch);
    }

    void Unlock() override {
        if (sdlTexture == nullptr) {
            return;
        }

        SDL_UnlockTexture(sdlTexture);
    }

};

int main(int argc, char** argv) {
    SDL_Init(0);

    SDL_Window* window = SDL_CreateWindow(
        "RT Everywhere (SDL)",
        1024,
        768,
        SDL_WINDOW_RESIZABLE
    );

    if (window == NULL) {
        printf("FATAL: SDL Window was NULL!\n");
        return 1;
    }

    const char* sdl_driver_name = "Software";
    SDL_Renderer* renderer = SDL_CreateRenderer(window, sdl_driver_name);

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

    sdl_concurrency = SDL_GetNumLogicalCPUCores();
    actual_concurrency = sdl_concurrency / 2;

    // Create a temporary camera to get the default values

    // TODO: Setup state, camera, and scene

    rteSDLFramebuffer previewFB(preview_texture);

    rteState state;

    camera.transform.SetPosition(glm::vec3(0, 1, 0));

#ifdef RTEVERYWHERE_IMGUI
    ImGuiContext* imgui_context = ImGui::CreateContext();
    ImGui::SetCurrentContext(imgui_context);

    ImGuiIO& io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;

    ImGui_ImplSDL3_InitForSDLRenderer(window, renderer);
    ImGui_ImplSDLRenderer3_Init(renderer);
#endif

    SDL_Event event;
    int run = 1;

    int lock_cursor = 0;
    int recreate_texture = 1;

    uint32_t last;
    uint32_t now = (uint32_t)SDL_GetTicks();

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
        int draw_preview = 1;

        //https://gamedev.stackexchange.com/questions/110825/how-to-calculate-delta-time-with-sdl
        last = now;
        now = (uint32_t)SDL_GetPerformanceCounter();
        float delta_time = (float) (now - last) / (float) SDL_GetPerformanceFrequency();

        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_EVENT_QUIT) {
                run = 0;
                break;
            }

            if (event.type == SDL_EVENT_WINDOW_RESIZED) {
                recreate_texture = 1;
            }

            if (event.type == SDL_EVENT_KEY_DOWN) {
                if (event.key.scancode == SDL_SCANCODE_W) {
                    w_down = 1;
                }

                if (event.key.scancode == SDL_SCANCODE_S) {
                    s_down = 1;
                }

                if (event.key.scancode == SDL_SCANCODE_A) {
                    a_down = 1;
                }

                if (event.key.scancode == SDL_SCANCODE_D) {
                    d_down = 1;
                }

                if (event.key.scancode == SDL_SCANCODE_Q) {
                    q_down = 1;
                }

                if (event.key.scancode == SDL_SCANCODE_E) {
                    e_down = 1;
                }
            }

            if (event.type == SDL_EVENT_KEY_UP) {
                if (event.key.scancode == SDL_SCANCODE_W) {
                    w_down = 0;
                }

                if (event.key.scancode == SDL_SCANCODE_S) {
                    s_down = 0;
                }

                if (event.key.scancode == SDL_SCANCODE_A) {
                    a_down = 0;
                }

                if (event.key.scancode == SDL_SCANCODE_D) {
                    d_down = 0;
                }

                if (event.key.scancode == SDL_SCANCODE_Q) {
                    q_down = 0;
                }

                if (event.key.scancode == SDL_SCANCODE_E) {
                    e_down = 0;
                }
            }

            if (event.type == SDL_EVENT_MOUSE_MOTION) {
                if (lock_cursor) {
                    static glm::vec3 euler;// = glm::degrees(camera.transform.GetEulerRotation());

                    euler.z = 0;
                    euler.y -= event.motion.xrel / 10.0F;
                    euler.x -= event.motion.yrel / 10.0F;

                    camera.transform.SetEulerRotation(glm::radians(euler));
                }
            }

            if (event.type == SDL_EVENT_MOUSE_BUTTON_DOWN) {
                if (event.button.button == SDL_BUTTON_RIGHT) {
                    lock_cursor = !lock_cursor;
                    SDL_SetWindowRelativeMouseMode(window, lock_cursor);

                    if (lock_cursor == 0) {
                        should_render = 1;
                    }
                }
            }

#ifdef RTEVERYWHERE_IMGUI
            ImGui_ImplSDL3_ProcessEvent(&event);
#endif
        }

        if (lock_cursor) {
            draw_preview = 1;
        }

        if ((w_down || s_down || a_down || d_down || q_down || e_down) && lock_cursor) {
            glm::vec3 movement;

            if (w_down) {
                movement.z = -1;
            }

            if (s_down) {
                movement.z = 1;
            }


            if (a_down) {
                movement.x = -1;
            }

            if (d_down) {
                movement.x = 1;
            }


            if (q_down) {
                movement.y = -1;
            }

            if (e_down) {
                movement.y = 1;
            }

            SDL_Keymod mod = SDL_GetModState();

            float gear = 1.0F;

            if (mod & (SDL_KMOD_LSHIFT | SDL_KMOD_RSHIFT))
                gear = 4.0F;

            glm::mat4x4 matrix = camera.transform.GetMatrix();
            //matrix = glm::inverse(matrix);

            movement = glm::normalize(movement);
            movement = matrix * glm::vec4(movement, 0.0F);
            movement *= gear * delta_time;

            camera.transform.SetPosition(camera.transform.GetPosition() + movement);

            // Transform movement vector to be local
        }

#ifdef RTEVERYWHERE_IMGUI
        static bool renderTabOpen = true;

        ImGui_ImplSDL3_NewFrame();
        ImGui_ImplSDLRenderer3_NewFrame();
        ImGui::NewFrame();

        ImGui::Begin("Render", &renderTabOpen);

        if (ImGui::Button("TEST RENDER") || lock_cursor || should_render) {

            rteRenderTarget rt;

            rt.pFramebuffer = &previewFB;
            rt.camera = camera;

            state.Render(rt);
            should_render = 0;

        }

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

            //ImGui::Checkbox("ACES Tonemap?", reinterpret_cast<bool*>(&tonemapping));

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

        if (ImGui::CollapsingHeader("Camera")) {
            ImGui::Indent();

            glm::vec3 position = camera.transform.GetPosition();
            glm::quat rotation = camera.transform.GetRotation();

            if (ImGui::DragFloat3("Position", reinterpret_cast<float*>(&position), 0.1F, -1000, 1000)) {
                camera.transform.SetPosition(position);
                should_render = 1;
            }

            if (ImGui::DragFloat4("Rotation", reinterpret_cast<float*>(&rotation), 0.1F, -1000, 1000)) {
                camera.transform.SetRotation(rotation);
                should_render = 1;
            }

            if (ImGui::DragFloat("Vertical FOV", &camera.fovY, 0.1F, 10, 120)) {
                should_render = 1;
            }

            ImGui::Unindent();
        }

        if (ImGui::CollapsingHeader("Scene")) {
            ImGui::Indent();

            if (ImGui::DragInt("Max Mirror Bounces", &state.maxNumMirrorBounces, 1.0F, 1, 10)) {
                should_render = 1;
            }

            // TODO: There was a sun editor here, that's no longer possible
            // Please replace it with a light editor, as the renderer should by then support generic light sources

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
        SDL_RenderTexture(renderer, target_texture, nullptr, nullptr);

#ifdef RTEVERYWHERE_IMGUI
        ImGui::Render();
        ImGui_ImplSDLRenderer3_RenderDrawData(ImGui::GetDrawData(), renderer);
#endif

        SDL_RenderPresent(renderer);
    }

    return 0;
}