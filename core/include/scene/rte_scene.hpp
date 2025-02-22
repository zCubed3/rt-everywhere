//
// Copyright (c) 2025 Liam Reese
//

#ifndef RTEVERYWHERE_RTE_SCENE_HPP
#define RTEVERYWHERE_RTE_SCENE_HPP

#include <data/rte_ray.hpp>

// TODO: Stupid, move this out
struct rteFragment {

public:
    glm::vec3 position;
    glm::vec3 normal;

    float depth;

    glm::vec4 debugColor;

public:
    rteFragment() = default;

    rteFragment(float far) : depth(far) {

    }

};

class rteScene {

protected:
    rteFragment TraceGround(const rteRay& ray);

public:
    bool hasGroundPlane = true;

    rteFragment TraceRay(const rteRay& ray);

};

#endif //RTEVERYWHERE_RTE_SCENE_HPP
