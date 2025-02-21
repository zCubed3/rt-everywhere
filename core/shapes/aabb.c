//
// Copyright (c) 2023 Liam R. (zCubed3)
//

#include "aabb.h"

int aabb_ray_intersect(rte_aabb_t aabb, rte_ray_t ray, real_t tmin, real_t tmax) {
    rvec3_t invD;

    rvec3_t t0s;
    rvec3_t t1s;

    rvec3_rcp(RVEC_OUT(invD), ray.direction);

    rvec3_sub(RVEC_OUT(t0s), aabb.min, ray.origin);
    rvec3_mul(RVEC_OUT(t0s), t0s, invD);

    rvec3_sub(RVEC_OUT(t1s), aabb.max, ray.origin);
    rvec3_mul(RVEC_OUT(t1s), t1s, invD);

    rvec3_t tsmaller;
    rvec3_t tbigger;

    rvec3_min(RVEC_OUT(tsmaller), t0s, t1s);
    rvec3_max(RVEC_OUT(tbigger), t0s, t1s);

    tmin = real_max(tmin, real_max(tsmaller[0], real_max(tsmaller[1], tsmaller[2])));
    tmax = real_min(tmax, real_min(tbigger[0], real_min(tbigger[1], tbigger[2])));

    return (tmin < tmax);
}