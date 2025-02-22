//
// Copyright (c) 2025 Liam Reese
//

#ifndef RTEVERYWHERE_RTE_AABB_H
#define RTEVERYWHERE_RTE_AABB_H

#include <glm/glm.hpp>

/// @brief Represents an axis aligned bounding box, used to accelerate ray lookups
struct rteAABB {
    glm::vec3 min;
    glm::vec3 max;
};

#endif //RTEVERYWHERE_RTE_AABB_H
