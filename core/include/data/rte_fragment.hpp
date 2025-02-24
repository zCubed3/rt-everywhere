//
// Copyright (c) 2025 Liam Reese
//

#ifndef RTEVERYWHERE_RTE_FRAGMENT_HPP
#define RTEVERYWHERE_RTE_FRAGMENT_HPP

#include <glm/glm.hpp>

#include <data/rte_ray.hpp>

struct rteFragment {

public:
    glm::vec3 position = glm::vec3(0, 0, 0);
    glm::vec3 normal = glm::vec3(0, 0, 0);

    glm::vec3 shaded = glm::vec3(0, 0, 0); //< The shaded color of this fragment

    float depth = 9999999;

    glm::vec4 debugColor;

    // TODO: Temporary, will be replaced with a tag that can be passed into lua and C++
    int materialIdx = -1;

    rteRay hitRay;

public:
    rteFragment() = default;

    rteFragment(float far) : depth(far) {

    }

    static const rteFragment INVALID_FRAGMENT;

};

#endif //RTEVERYWHERE_RTE_FRAGMENT_HPP
