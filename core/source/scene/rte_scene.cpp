//
// Copyright (c) 2025 Liam Reese
//

#include <scene/rte_scene.hpp>

#include <scene/rte_primitive.hpp>
#include <scene/rte_aabb.hpp>

#include <rte_state.hpp>
#include "tracy/Tracy.hpp"

rteFragment rteScene::TraceGround(const rteRay &ray) const {

    ZoneScopedN("Trace Ground");

    float groundDistance = -ray.origin.y / ray.direction.y;

    if (groundDistance < 0)
        return rteFragment::INVALID_FRAGMENT;

    rteFragment fragment;

    fragment.position = ray.origin + (ray.direction * groundDistance);
    fragment.normal = glm::vec3(0, 1, 0);

    fragment.depth = groundDistance;

    fragment.debugColor = glm::floor(glm::vec4(fragment.position * 100.0F, 1.0F));
    fragment.materialIdx = 1;

    return fragment;

}

void rteScene::TraceSky(const rteRay &ray, rteFragment& fragment) const {

    ZoneScopedN("Trace & Shade Sky");

    float skyPhi = glm::dot(ray.direction, glm::vec3(0, 1, 0));
    skyPhi = glm::clamp(skyPhi, 0.0F, 1.0F);
    skyPhi = glm::pow(skyPhi, 0.35F);

    fragment.depth = farClip;
    fragment.shaded = glm::vec4(skyColor * skyPhi, 1);
    fragment.materialIdx = 0;

}

void rteScene::BounceMirror(const rteState* state, const rteRay &ray, rteFragment &fragment) const {

    ZoneScopedN("Bounce Mirror");

    rteRay reflected;
    reflected.origin = (fragment.position + fragment.normal * 0.0001F);
    reflected.direction = glm::reflect(ray.direction, -fragment.normal);
    reflected.bounces = ray.bounces + 1;

    rteFragment sceneRetrace;

    // TODO: Early exit
    bool hit = TraceScene(state, reflected, sceneRetrace);
    fragment.shaded *= sceneRetrace.shaded;

    // TODO: This is stupid
    fragment.hitRay = sceneRetrace.hitRay;

}

void rteScene::ShadeFrag(const rteState* state, const rteRay& ray, rteFragment &fragment) const {

    rteShaderInput input {
        state,
        this,
        ray,
        fragment
    };

    ZoneScopedN("Shade Fragment");

    const glm::vec3 lightDir = glm::normalize(glm::vec3(1.0F, 1.0F, 1.0F));
    //glm::vec3 lightDir = glm::vec3(1, 1, 1) - fragment.position;
    //float lightDistance = glm::length(lightDir);
    //lightDir /= lightDistance;
    float lightDistance = 9999;

    if (fragment.materialIdx == 1) { // TODO: TEMP Ground
        /*
        glm::vec3 checker = glm::floor(fragment.position * groundCheckerSize);

        float mod = glm::mod(checker.x + glm::mod(checker.z, 2.0F), 2.0F);
        fragment.shaded = mod ? groundColor1 : groundColor2;

        float specular = mod ? groundSpecular1 : groundSpecular2;

        rteFragment metalFrag = fragment;
        metalFrag.shaded = glm::vec3(1, 1, 1);

        BounceMirror(state, ray, metalFrag);
        fragment.shaded = glm::mix(fragment.shaded, fragment.shaded * metalFrag.shaded, specular);
        fragment.hitRay = metalFrag.hitRay;
         */

        static const rteShader* shader = nullptr;

        if (shader == nullptr) {
            shader = state->GetShader("ground_shader");
        }

        shader->ShadeFragment(input, fragment);

        ZoneScopedN("Trace Shadow");

        rteFragment shadowFragment;
        rteRay shadowRay(fragment.position + fragment.normal * 0.001F, lightDir);

        float shadow = !TraceSceneSimple(shadowRay, shadowFragment);
        // Shadow OOR
        if (shadowFragment.depth < lightDistance) {
            fragment.shaded *= shadow;
        }
    }

    if (fragment.materialIdx == 2) { // TODO: TEMP Sphere
        //fragment.shaded = glm::vec3(1, 1, 1);
        //BounceMirror(state, ray, fragment);

        static const rteShader* shader = nullptr;

        if (shader == nullptr) {
            shader = state->GetShader("mirror");
        }

        shader->ShadeFragment(input, fragment);
    }

    if (fragment.materialIdx == 3) { // TODO: TEMP Sphere
        fragment.shaded = glm::vec3(1, 1, 1) * glm::clamp(glm::dot(fragment.normal, lightDir), 0.0F, 1.0F);

        ZoneScopedN("Trace Shadow");

        rteFragment shadowFragment;
        rteRay shadowRay(fragment.position + fragment.normal * 0.001F, lightDir);

        float shadow = !TraceSceneSimple(shadowRay, shadowFragment);

        // Shadow OOR
        if (shadowFragment.depth < lightDistance) {
            fragment.shaded *= shadow;
        }

        //BounceMirror(state, ray, fragment);
    }
}

bool rteScene::TraceSceneSimple(const rteRay &ray, rteFragment &fragment) const {
    if (ray.bounces >= numMirrorBounces) {
        // TODO: Is this bad?
        fragment = rteFragment::INVALID_FRAGMENT;
        fragment.hitRay = ray;

        return false;
    }

    bool hitSomething = false;
    TraceSky(ray, fragment);

    fragment.hitRay = ray;

    if (hasGroundPlane) {
        rteFragment groundFrag = TraceGround(ray);

        if (groundFrag.depth < fragment.depth) {
            hitSomething = true;
            fragment = groundFrag;
        }
    }

    const float SPHERE_RADIUS = 1.0F;
    const glm::vec3 SPHERE_ORIGIN = glm::vec3(0, SPHERE_RADIUS, -5);

    static bool constructedBVHs = false;
    static rteSphere SPHERES[3 * 3];
    static rteAABB BVHs[3 * 3];

    if (!constructedBVHs) {
        for (int x = 0; x < 3; x++) {
            for (int y = 0; y < 3; y++) {
                glm::vec3 sphereOrigin;
                sphereOrigin = SPHERE_ORIGIN + glm::vec3(x * (SPHERE_RADIUS * 3.0F), 0, y * -(SPHERE_RADIUS * 3.0F));

                rteAABB aabb;
                aabb.min = sphereOrigin - glm::vec3(SPHERE_RADIUS);
                aabb.max = sphereOrigin + glm::vec3(SPHERE_RADIUS);

                rteSphere sphere;

                sphere.radius = SPHERE_RADIUS;
                sphere.origin = sphereOrigin;

                SPHERES[(y * 3) + x] = sphere;
                BVHs[(y * 3) + x] = aabb;

                constructedBVHs = true;
            }
        }
    }

    for (int x = 0; x < 3; x++) {
        for (int y = 0; y < 3; y++) {
            float depth = farClip;
            if (BVHs[(y * 3) + x].IntersectRay(ray, depth, 0.001F, farClip) && depth < fragment.depth) {
                if (SPHERES[(y * 3) + x].TraceSphere(ray, fragment)) {
                    hitSomething = true;
                    fragment.materialIdx += (x + y) % 2 == 0;
                }
            }
        }
    }

    if (!hitSomething) { // Completely unshaded fragment, this was likely only the sky
        return false;
    }

    return fragment.depth <= farClip;
}

bool rteScene::TraceScene(const rteState* state, const rteRay &ray, rteFragment& fragment) const {

    ZoneScopedN("Trace Scene");
    bool result = TraceSceneSimple(ray, fragment);

    if (result) {
        ShadeFrag(state, ray, fragment);
    }

    return result;
}
