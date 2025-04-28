//
// Copyright (c) 2025 Liam Reese
//
#include <scene/rte_primitive.hpp>
#include "tracy/Tracy.hpp"

bool rteSphere::TraceSphere(const rteRay& ray, rteFragment& fragment) const {

    ZoneScopedN("Trace Sphere");

    // Ported from http://three-eyed-games.com/2018/05/03/gpu-ray-tracing-in-unity-part-1/
    glm::vec3 d = ray.origin - origin;

    float p1 = -glm::dot(ray.direction, d);
    float p1sqr = p1 * p1;

    float r2 = radius * radius;
    float p2sqr = p1sqr - glm::dot(d, d) + r2;

    if (p2sqr < 0) {
        return false;
    }

    float p2 = glm::sqrt(p2sqr);
    float t = p1 - p2 > 0 ? p1 - p2 : p1 + p2;

    if (t < 0) {
        return false;
    }

    if (t > fragment.depth) {
        return false; // Something is closer than this hit
    }

    fragment.position = ray.origin + (ray.direction * t);
    fragment.normal = glm::normalize(fragment.position - origin);
    fragment.depth = t;

    // TODO: TEMPORARY
    fragment.materialIdx = 2;

    return true;

}

