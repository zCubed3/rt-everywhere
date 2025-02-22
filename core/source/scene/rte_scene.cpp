//
// Copyright (c) 2025 Liam Reese
//

#include <scene/rte_scene.hpp>

rteFragment rteScene::TraceGround(const rteRay &ray) {
    float groundDistance = -ray.origin.y / ray.direction.y;

    if (groundDistance < 0)
        return rteFragment(999999);

    rteFragment fragment;

    fragment.position = ray.origin + (ray.direction * groundDistance);
    fragment.normal = glm::vec3(0, 1, 0);

    fragment.depth = groundDistance;

    fragment.debugColor = glm::floor(glm::vec4(fragment.position * 100.0F, 1.0F));

    return fragment;

}

rteFragment rteScene::TraceRay(const rteRay &ray) {

    rteFragment fragment;

    // TODO: Scene far or view far?
    const float FAR = 1000.0F;

    fragment.depth = FAR;

    float skyPhi = glm::dot(ray.direction, glm::vec3(0, 0, 0));
    skyPhi = glm::clamp(skyPhi, 0.0F, 1.0F);
    fragment.debugColor = glm::vec4(skyPhi, 0, 0, 1);


    if (hasGroundPlane) {
        rteFragment groundFrag = TraceGround(ray);

        if (groundFrag.depth < fragment.depth)
            fragment = groundFrag;
    }

    return fragment;

}