//
// Copyright (c) 2025 Liam Reese
//

#ifndef RTEVERYWHERE_RAY_HPP
#define RTEVERYWHERE_RAY_HPP

#include <glm/glm.hpp>

struct rteRay {
    /// @brief Where the ray starts in space
    glm::vec3 origin = glm::vec3(0.0F);

    /// @brief What direction the ray will move in
    glm::vec3 direction = glm::vec3(0.0F, 0.0F, 1.0F);

    // TODO: Make this debug only?
    int bounces = 0;

    rteRay() = default;
    rteRay(const glm::vec3& origin, const glm::vec3& direction) : origin(origin), direction(direction) {}
};

#endif //RTEVERYWHERE_RAY_HPP
