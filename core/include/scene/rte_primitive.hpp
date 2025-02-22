//
// Copyright (c) 2025 Liam Reese
//

#ifndef RTEVERYWHERE_RTE_PRIMITIVE_HPP
#define RTEVERYWHERE_RTE_PRIMITIVE_HPP

#include <data/rte_ray.hpp>
#include <data/rte_fragment.hpp>

struct rteSphere {

    glm::vec3 origin;
    float radius;

    rteFragment TraceSphere(const rteRay& ray) const;

};

#endif //RTEVERYWHERE_RTE_PRIMITIVE_HPP
