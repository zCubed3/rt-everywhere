//
// Copyright (c) 2025 Liam Reese
//

#ifndef RTEVERYWHERE_RTE_LIGHT_HPP
#define RTEVERYWHERE_RTE_LIGHT_HPP

#include <glm/glm.hpp>

enum class rteLightType {
    Directional, Point, Spot
};

/// @brief A light within a scene
struct rteLight {

public:
    bool castShadows = true;

    // TODO: Shadow radius?

    float range = 1.0F;

    /// @brief The color of this light, it's HDR so intensity is part of the actual color
    glm::vec3 color;

};

#endif //RTEVERYWHERE_RTE_LIGHT_HPP
