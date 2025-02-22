//
// Copyright (c) 2025 Liam Reese
//

#include <rte_state.hpp>

#include <pipeline/rte_framebuffer.hpp>
#include <scene/rte_camera.hpp>

#include <sol/sol.hpp>

void rteState::LoadLuaModules() {

    // TODO: Use lua for scene descriptions

}

void rteState::Render(rteRenderTarget rt) {

    if (rt.pFramebuffer == nullptr) {
        return;
    }

    rt.pFramebuffer->Lock();

    // After locking the framebuffer we start tracing the scene line by line
    int width = rt.pFramebuffer->GetWidth();
    int height = rt.pFramebuffer->GetHeight();

    rt.camera.aspect = (float)rt.pFramebuffer->GetWidth() / (float)rt.pFramebuffer->GetHeight();

    rt.camera.ComputeMatrix();

    rteRay ray;
    ray.origin = rt.camera.transform.GetPosition();

    for (int y = 0; y < height; y++) {
        float v = y / (height - 1.0F);

        for (int x = 0; x < width; x++) {
            float u = x / (width - 1.0F);

            ray.direction = rt.camera.NDCToRayDirection(glm::vec2(u, v));

            rteFragment fragment = scene.TraceScene(ray, maxNumMirrorBounces);

            //rt.pFramebuffer->WritePixel(x, y, fragment.debugColor);
            rt.pFramebuffer->WritePixel(x, y, glm::vec4(fragment.shaded, 1.0F));
        }
    }

    rt.pFramebuffer->Unlock();

}
