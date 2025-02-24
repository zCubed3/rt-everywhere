//
// Copyright (c) 2025 Liam Reese
//

#ifndef RTEVERYWHERE_RTE_AABB_H
#define RTEVERYWHERE_RTE_AABB_H

#include <glm/glm.hpp>

#include <data/rte_ray.hpp>

/// @brief Represents an axis aligned bounding box, used to accelerate ray lookups
struct rteAABB {
    glm::vec3 min;
    glm::vec3 max;

    bool IntersectRay(const rteRay &ray, float& hitT, float tMin, float tMax);
};

#endif //RTEVERYWHERE_RTE_AABB_H
