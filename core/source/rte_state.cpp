//
// Copyright (c) 2025 Liam Reese
//

#include <rte_state.hpp>

#include <pipeline/rte_framebuffer.hpp>
#include <scene/rte_camera.hpp>

#include <sol/sol.hpp>

void rteState::LoadLuaModules() {

    // TODO: Use lua for scene descriptions
    luaState = sol::state();

    // TODO: Organize this stuff better

    //
    // GLM types
    //
    luaState.new_usertype<glm::vec2>(
        "vec2",

        sol::constructors<glm::vec2()>(),
        sol::constructors<glm::vec2(), float>(),
        sol::constructors<glm::vec2(), float, float>(),

        "x", &glm::vec2::x,
        "y", &glm::vec2::y

        // TODO: Glue math functions
    );

    luaState.new_usertype<glm::vec3>(
        "vec3",

        sol::constructors<glm::vec3()>(),
        sol::constructors<glm::vec3(), float>(),
        sol::constructors<glm::vec3(), float, float, float>(),

        "x", &glm::vec3::x,
        "y", &glm::vec3::y,
        "z", &glm::vec3::z

        // TODO: Glue math functions
    );

    luaState.new_usertype<glm::vec4>(
        "vec4",

        sol::constructors<glm::vec4()>(),
        sol::constructors<glm::vec4(), float>(),
        sol::constructors<glm::vec4(), float, float, float, float>(),

        "x", &glm::vec4::x,
        "y", &glm::vec4::y,
        "z", &glm::vec4::z,
        "w", &glm::vec4::w

        // TODO: Glue math functions
    );

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

            const bool visualizeBounceHeat = false;

            rteFragment fragment;
            scene.TraceScene(ray, fragment);

            if (visualizeBounceHeat) {
                float bounceHeat = fragment.hitRay.bounces / (float) scene.numMirrorBounces;
                fragment.shaded = glm::vec3(bounceHeat);

                // TODO: Bounces shouldn't go OOR anymore
                fragment.shaded.r = (float) (fragment.hitRay.bounces > scene.numMirrorBounces);
            }

            //rt.pFramebuffer->WritePixel(x, y, fragment.debugColor);
            rt.pFramebuffer->WritePixel(x, y, glm::vec4(fragment.shaded, 1.0F));
        }
    }

    rt.pFramebuffer->Unlock();

}
