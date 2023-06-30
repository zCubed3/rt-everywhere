/****************************************************************************************/
/* Copyright (c) 2023 zCubed3														*/
/* 																						*/
/* Permission is hereby granted, free of charge, to any person obtaining a copy			*/
/* of this software and associated documentation files (the "Software"), to deal		*/
/* in the Software without restriction, including without limitation the rights			*/
/* to use, copy, modify, merge, publish, distribute, sublicense, and/or sell			*/
/* copies of the Software, and to permit persons to whom the Software is				*/
/* furnished to do so, subject to the following conditions:								*/
/* 																						*/
/* The above copyright notice and this permission notice shall be included in all		*/
/* copies or substantial portions of the Software.										*/
/* 																						*/
/* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR			*/
/* IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,				*/
/* FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE			*/
/* AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER				*/
/* LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,		*/
/* OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE		*/
/* SOFTWARE.																			*/
/****************************************************************************************/

#include "sphere.h"

#include <math.h>

int sphere_ray_intersect(sphere_t sphere, ray_t ray, sphere_intersect_t* intersect) {
	rvec3_copy(intersect->point, (rvec3_t){0, 0, 0});
	rvec3_copy(intersect->normal, (rvec3_t){0, 0, 0});
	intersect->distance = 0;

	// Ported to C from http://three-eyed-games.com/2018/05/03/gpu-ray-tracing-in-unity-part-1/
	rvec3_t d;
	rvec3_sub(d, ray.origin, sphere.origin);

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
		rvec3_mul_scalar(intersect->point, ray.direction, t);
		rvec3_add(intersect->point, ray.origin, intersect->point);

		// Then the normal
		rvec3_sub(intersect->normal, intersect->point, sphere.origin);
		rvec3_normalize(intersect->normal);

		// Then the travel
		rvec3_t travel;
		rvec3_sub(travel, intersect->point, ray.origin);
		intersect->distance = rvec3_length(travel);

		return 1;
	}

	return 0;
}