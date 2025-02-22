//
// Copyright (c) 2025 Liam Reese
//

#ifndef RTEVERYWHERE_RAY_HPP
#define RTEVERYWHERE_RAY_HPP

#include <glm/glm.hpp>

struct rteRay {
    /// @brief Where the ray starts in space
    glm::vec3 origin;

    /// @brief What direction the ray will move in
    glm::vec3 direction;
};

#endif //RTEVERYWHERE_RAY_HPP
