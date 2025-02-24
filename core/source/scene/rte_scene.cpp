//
// Copyright (c) 2025 Liam Reese
//

#include <scene/rte_scene.hpp>

#include <scene/rte_primitive.hpp>
#include <scene/rte_aabb.hpp>

rteFragment rteScene::TraceGround(const rteRay &ray) {
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

void rteScene::TraceSky(const rteRay &ray, rteFragment& fragment) {

    float skyPhi = glm::dot(ray.direction, glm::vec3(0, 1, 0));
    skyPhi = glm::clamp(skyPhi, 0.0F, 1.0F);

    fragment.depth = farClip;
    fragment.shaded = glm::vec4(skyColor * skyPhi, 1);
    fragment.materialIdx = 0;

}

void rteScene::BounceMirror(const rteRay &ray, rteFragment &fragment) {

    rteRay reflected;
    reflected.origin = (fragment.position + fragment.normal * 0.0001F);
    reflected.direction = glm::reflect(ray.direction, -fragment.normal);
    reflected.bounces = ray.bounces + 1;

    rteFragment sceneRetrace;

    // TODO: Early exit
    bool hit = TraceScene(reflected, sceneRetrace);
    fragment.shaded *= sceneRetrace.shaded;

    // TODO: This is stupid
    fragment.hitRay = sceneRetrace.hitRay;

}

void rteScene::ShadeFrag(const rteRay& ray, rteFragment &fragment) {
    if (fragment.materialIdx == 1) { // TODO: TEMP Ground

        glm::vec3 checker = glm::floor(fragment.position * groundCheckerSize);

        float mod = glm::mod(checker.x + glm::mod(checker.z, 2.0F), 2.0F);
        fragment.shaded = mod ? groundColor1 : groundColor2;

        float specular = mod ? groundSpecular1 : groundSpecular2;

        rteFragment metalFrag = fragment;
        metalFrag.shaded = glm::vec3(1, 1, 1);

        BounceMirror(ray, metalFrag);
        fragment.shaded = glm::mix(fragment.shaded, fragment.shaded * metalFrag.shaded, specular);
        fragment.hitRay = metalFrag.hitRay;
    }

    if (fragment.materialIdx == 2) { // TODO: TEMP Sphere
        fragment.shaded = glm::vec3(1, 1, 1);

        BounceMirror(ray, fragment);
    }
}

bool rteScene::TraceScene(const rteRay &ray, rteFragment& fragment) {

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

    const float SPHERE_RADIUS = 2.0F;
    const glm::vec3 SPHERE_ORIGIN = glm::vec3(0, SPHERE_RADIUS, -5);

    rteAABB aabb;
    aabb.min = SPHERE_ORIGIN - glm::vec3(SPHERE_RADIUS);
    aabb.max = SPHERE_ORIGIN + glm::vec3(SPHERE_RADIUS);

    if (aabb.IntersectRay(ray, 0.001F, farClip)) {
        rteSphere sphere;

        sphere.radius = SPHERE_RADIUS;
        sphere.origin = SPHERE_ORIGIN;

        if (sphere.TraceSphere(ray, fragment))
            hitSomething = true;
    }

    if (!hitSomething) { // Completely unshaded fragment, this was likely only the sky
        return false;
    }

    // Shade fragment
    ShadeFrag(ray, fragment);

    return fragment.depth <= farClip;

}