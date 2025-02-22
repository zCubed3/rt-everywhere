//
// Copyright (c) 2025 Liam Reese
//
#include <scene/rte_primitive.hpp>

rteFragment rteSphere::TraceSphere(const rteRay& ray) const {

    rteFragment fragment(9999999);

    // Ported from http://three-eyed-games.com/2018/05/03/gpu-ray-tracing-in-unity-part-1/
    glm::vec3 d = ray.origin - origin;

    float p1 = -glm::dot(ray.direction, d);
    float p1sqr = p1 * p1;

    float r2 = radius * radius;
    float p2sqr = p1sqr - glm::dot(d, d) + r2;

    if (p2sqr < 0)
        return fragment;

    float p2 = glm::sqrt(p2sqr);
    float t = p1 - p2 > 0 ? p1 - p2 : p1 + p2;

    if (t > 0) {
        fragment.position = ray.origin + (ray.direction * t);
        fragment.normal = glm::normalize(fragment.position - origin);
        fragment.depth = t;
    }

    return fragment;

}

