/* Copyright (c) 2023-2025 Liam R. (zCubed3)                                                 */

#include "sphere.h"

#include <math.h>

int sphere_ray_intersect(sphere_t sphere, rte_ray_t ray, sphere_intersect_t* intersect) {
	rvec3_copy(RVEC_OUT(intersect->point), (rvec3_t){0, 0, 0});
	rvec3_copy(RVEC_OUT(intersect->normal), (rvec3_t){0, 0, 0});
	intersect->distance = 0;

	// Ported to C from http://three-eyed-games.com/2018/05/03/gpu-ray-tracing-in-unity-part-1/
	rvec3_t d;
	rvec3_sub(RVEC_OUT(d), ray.origin, sphere.origin);

	real_t p1 = -rvec3_dot(ray.direction, d);
	real_t p1sqr = p1 * p1;

	real_t r2 = sphere.radius * sphere.radius;
	real_t p2sqr = p1sqr - rvec3_dot(d, d) + r2;

	if (p2sqr < 0)
		return 0;

	real_t p2 = (real_t)sqrt(p2sqr);
	real_t t = p1 - p2 > 0 ? p1 - p2 : p1 + p2;

	if (t > 0) {
		// Calculate the point
		rvec3_mul_scalar(RVEC_OUT(intersect->point), ray.direction, t);
		rvec3_add(RVEC_OUT(intersect->point), ray.origin, intersect->point);

		// Then the normal
		rvec3_sub(RVEC_OUT(intersect->normal), intersect->point, sphere.origin);
		rvec3_normalize(RVEC_OUT(intersect->normal));

		// Then the travel
		//rvec3_t travel;
		//rvec3_sub(travel, intersect->point, ray.origin);
		//intersect->distance = rvec3_length(travel);
        intersect->distance = t;

		return 1;
	}

	return 0;
}