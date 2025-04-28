//
// Copyright (c) 2025 Liam Reese
//

#include <scene/rte_aabb.hpp>
#include "tracy/Tracy.hpp"

bool rteAABB::IntersectRay(const rteRay &ray, float& hitT, float tMin, float tMax) {
    ZoneScopedN("Trace AABB");

    glm::vec3 invD, t0s, t1s;

    invD = 1.0F / ray.direction;

    t0s = (this->min - ray.origin) * invD;
    t1s = (this->max - ray.origin) * invD;

    glm::vec3 tsmaller, tbigger;

    tsmaller = glm::min(t0s, t1s);
    tbigger = glm::max(t0s, t1s);

    tMin = glm::max(tMin, glm::max(tsmaller[0], glm::max(tsmaller[1], tsmaller[2])));
    tMax = glm::min(tMax, glm::min(tbigger[0], glm::min(tbigger[1], tbigger[2])));

    hitT = tMin;
    return (tMin < tMax);
}