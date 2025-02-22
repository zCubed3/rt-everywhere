//
// Copyright (c) 2023 Liam R. (zCubed3)
//

#ifndef RTEVERYWHERE_SPHERE_H
#define RTEVERYWHERE_SPHERE_H

#include "../math/real.h"
#include "../math/vectors.h"
#include "../math/ray.h"

typedef struct sphere {
    real_t radius;
    rvec3_t origin;
    rvec3_t color;
    int type; // Material type
} sphere_t;

typedef struct sphere_intersect {
    rvec3_t point;
    rvec3_t normal;
    real_t distance;
} sphere_intersect_t;

extern int sphere_ray_intersect(sphere_t sphere, rte_ray_t ray, sphere_intersect_t* intersect);

#endif //RTEVERYWHERE_SPHERE_H
