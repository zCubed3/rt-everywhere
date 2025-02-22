//
// Copyright (c) 2025 Liam Reese
//

#include <scene/rte_scene.hpp>

#include <scene/rte_primitive.hpp>

rteFragment rteScene::TraceGround(const rteRay &ray) {
    float groundDistance = -ray.origin.y / ray.direction.y;

    if (groundDistance < 0)
        return rteFragment(999999);

    rteFragment fragment;

    fragment.position = ray.origin + (ray.direction * groundDistance);
    fragment.normal = glm::vec3(0, 1, 0);

    fragment.depth = groundDistance;

    fragment.debugColor = glm::floor(glm::vec4(fragment.position * 100.0F, 1.0F));
    fragment.materialIdx = 1;

    return fragment;

}

rteFragment rteScene::TraceSky(const rteRay &ray) {

    rteFragment fragment(farClip);

    float skyPhi = glm::dot(ray.direction, glm::vec3(0, 1, 0));
    skyPhi = glm::clamp(skyPhi, 0.0F, 1.0F);

    fragment.shaded = glm::vec4(skyColor * skyPhi, 1);
    fragment.materialIdx = 0;

    return fragment;

}

rteFragment rteScene::TraceScene(const rteRay &ray, int recursion) {

    if (recursion < 0) {
        return rteFragment(farClip);
    }

    rteFragment fragment = TraceSky(ray);

    if (hasGroundPlane) {
        rteFragment groundFrag = TraceGround(ray);

        if (groundFrag.depth < fragment.depth) {
            fragment = groundFrag;
        }
    }

    rteSphere sphere;

    sphere.radius = 2.0F;
    sphere.origin = glm::vec3(0, sphere.radius * 0.5F, -5);

    rteFragment sphereFrag = sphere.TraceSphere(ray);

    if (sphereFrag.depth < fragment.depth) {
        sphereFrag.materialIdx = 2;
        fragment = sphereFrag;
    }

    if (fragment.depth >= farClip) {
        recursion = -1;
        return fragment; // Fragment not shaded!
    }

    // Shade fragment
    float shadePhi = glm::dot(fragment.normal, glm::vec3(0, 1, 0));
    fragment.shaded = glm::vec3(1, 1, 1) * shadePhi;

    if (fragment.materialIdx == 1) { // TODO: TEMP Ground

        glm::vec3 checker = glm::floor(fragment.position * groundCheckerSize);

        float mod = glm::mod(checker.x + glm::mod(checker.z, 2.0F), 2.0F);
        fragment.shaded = mod ? groundColor1 : groundColor2;

        float specular = mod ? groundSpecular1 : groundSpecular2;

        rteRay reflected;
        reflected.origin = (fragment.position + fragment.normal * 0.0001F);
        reflected.direction = glm::reflect(ray.direction, -fragment.normal);

        fragment.shaded = glm::mix(fragment.shaded, fragment.shaded * TraceScene(reflected, --recursion).shaded, specular);

    }

    if (fragment.materialIdx == 2) { // TODO: TEMP Sphere
        rteRay reflected;
        reflected.direction = glm::reflect(ray.direction, fragment.normal);
        reflected.origin = (fragment.position + fragment.normal * 0.0001F);

        fragment.shaded = glm::vec3(1, 1, 1);
        fragment.shaded = fragment.shaded * TraceScene(reflected, --recursion).shaded;
    }

    return fragment;

}