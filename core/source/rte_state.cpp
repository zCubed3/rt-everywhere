//
// Copyright (c) 2025 Liam Reese
//

#include <rte_state.hpp>

#include <pipeline/rte_framebuffer.hpp>
#include <scene/rte_camera.hpp>

#include <pipeline/rte_default_shader.hpp>
#include <pipeline/rte_lua_shader.hpp>
#include <pipeline/rte_mirror_shader.hpp>
#include <pipeline/rte_ground_shader.hpp>

#include <iostream>
#include <chrono>

void rteState::LoadLuaModules() {

    // TODO: Use lua for scene descriptions
}

void rteState::Setup() {

    RegisterShader(new rteDefaultShader());
    RegisterShader(new rteMirrorShader());
    RegisterShader(new rteGroundShader());

    LoadLuaModules();

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

    // TODO: Only for profiling
    uint64_t* shadeTimes = new uint64_t[height * width];
    highestShadeTime = 0;

    const float xPer = 1.0F / (width - 1.0F);
    const float yPer = 1.0F / (height - 1.0F);

    for (int y = 0; y < height; y++) {
        float v = y * yPer;

        for (int x = 0; x < width; x++) {
            float u = x * xPer;

            auto start = std::chrono::high_resolution_clock::now();

            ray.direction = rt.camera.NDCToRayDirection(glm::vec2(u, v));

            rteFragment fragment;
            scene.TraceScene(this, ray, fragment);

            if (visualizeBounceHeat) {
                float bounceHeat = fragment.hitRay.bounces / (float) scene.numMirrorBounces;
                fragment.shaded = glm::vec3(bounceHeat);

                // TODO: Bounces shouldn't go OOR anymore
                fragment.shaded.r = (float) (fragment.hitRay.bounces > scene.numMirrorBounces);
            }

            //rt.pFramebuffer->WritePixel(x, y, fragment.debugColor);
            rt.pFramebuffer->WritePixel(x, y, glm::vec4(fragment.shaded, 1.0F));

            auto end = std::chrono::high_resolution_clock::now();

            auto dur = end - start;

            uint64_t shadeTime = std::chrono::duration_cast<std::chrono::nanoseconds>(dur).count();
            shadeTimes[(y * width) + x] = shadeTime;

            if (shadeTime > highestShadeTime)
                highestShadeTime = shadeTime;
        }
    }

    if (visualizeShadeTime) {
        for (int y = 0; y < height; y++) {
            for (int x = 0; x < width; x++) {
                float shadeTime = (float)((double)shadeTimes[(y * width) + x] / (double)highestShadeTime);
                shadeTime = glm::pow(shadeTime, 0.3F);

                rt.pFramebuffer->WritePixel(x, y, glm::vec4(shadeTime, shadeTime, shadeTime, 1.0F));
            }
        }
    }

    delete[] shadeTimes;

    rt.pFramebuffer->Unlock();

}

void rteState::RegisterShader(rteShader* shader) {
    // TODO: If debug / verbose
    std::cout << "[RTE]: Registered shader '" << shader->GetShaderID() << "'" << std::endl;

    this->loadedShaders[shader->GetShaderID()] = shader;
}
