//
// Copyright (c) 2023 Liam R. (zCubed3)
//

#ifndef RTEVERYWHERE_AABB_H
#define RTEVERYWHERE_AABB_H

#include "../math/real.h"
#include "../math/vectors.h"
#include "../math/ray.h"

typedef struct rte_aabb {
    rvec3_t min;
    rvec3_t max;
} rte_aabb_t;

extern int aabb_ray_intersect(rte_aabb_t sphere, rte_ray_t ray, real_t tmin, real_t tmax);

#endif //RTEVERYWHERE_AABB_H
