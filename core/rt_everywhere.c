/****************************************************************************************/
/* Copyright (c) 2023 zCubed3                                                        */
/*                                                                                      */
/* Permission is hereby granted, free of charge, to any person obtaining a copy         */
/* of this software and associated documentation files (the "Software"), to deal        */
/* in the Software without restriction, including without limitation the rights         */
/* to use, copy, modify, merge, publish, distribute, sublicense, and/or sell            */
/* copies of the Software, and to permit persons to whom the Software is                */
/* furnished to do so, subject to the following conditions:                             */
/*                                                                                      */
/* The above copyright notice and this permission notice shall be included in all       */
/* copies or substantial portions of the Software.                                      */
/*                                                                                      */
/* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR           */
/* IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,             */
/* FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE          */
/* AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER               */
/* LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,        */
/* OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE        */
/* SOFTWARE.                                                                            */
/****************************************************************************************/

#include "rt_everywhere.h"

void screen_to_viewport(rvec2_t dst, viewport_t viewport, point_t point) {
	// Note: When x == 0, x / width = 0, but x never hits width
	// Therefore we must add half the texel size to x to account for this
	real_t tex_x = REAL(1.0) / (real_t)viewport.width;
	real_t tex_y = REAL(1.0) / (real_t)viewport.height;

	real_t u = (real_t)point.x / (real_t)viewport.width;
	real_t v = (real_t)point.y / (real_t)viewport.height;

	dst[0] = ((u + (tex_x / REAL(2.0))) * REAL(2.0)) - REAL(1.0);
	dst[1] = ((v + (tex_y / REAL(2.0))) * REAL(2.0)) - REAL(1.0);
}

camera_t setup_camera(viewport_t viewport, rvec3_t position, rvec3_t rotation) {
	camera_t cam;

	rmat4_t mat_translation;
	rmat4_t mat_rotation;

	rmat4_translate(mat_translation, position);
	rmat4_rotate(mat_rotation, rotation);

	rmat4_mul(cam.mat_v, mat_translation, mat_rotation);

	real_t aspect = (real_t)viewport.width / (real_t)viewport.height;
	rmat4_perspective(cam.mat_p, REAL(90.0), aspect, REAL(0.01), REAL(100.0));

	rmat4_t mat_vp;

	rmat4_mul(mat_vp, cam.mat_p, cam.mat_v);
	rmat4_inverse(cam.mat_vp_i, mat_vp);

	cam.viewport = viewport;

	rvec3_copy(cam.position, position);
	rvec3_copy(cam.rotation, rotation);

	return cam;
}

void trace_scene(rvec3_t dst_col, camera_t camera, point_t point) {
	// Set the color to zero
	rvec3_copy(dst_col, (rvec3_t){0, 0, 0});

	rvec2_t view_coord;
	screen_to_viewport(view_coord, camera.viewport, point);

	// Our camera is centered at 0, 0, -1
	// The y-axis is flipped if necessary
	ray_t ray;

	rvec3_copy(ray.direction, (rvec3_t){view_coord[0], -view_coord[1], 1});

#ifdef RTEVERYWHERE_FLIP_Y
	ray.direction[1] *= -1;
#endif

	//rvec3_normalize(ray.direction);

	rvec4_t pre_t;
	rvec4_t post_t;

	rvec4_copy_rvec3_w(pre_t, ray.direction, REAL(1.0));
	rmat4_mul_rvec4(post_t, camera.mat_vp_i, pre_t);

	rvec3_copy_rvec4(ray.direction, post_t);
	rvec3_normalize(ray.direction);

	rvec4_copy_rvec3_w(pre_t, (rvec3_t){0, 0, 0}, REAL(1.0));
	rmat4_mul_rvec4(post_t, camera.mat_v, pre_t);

	rvec3_copy_rvec4(ray.origin, post_t);

	// Intersect the ground
	real_t closest_t = 100;
	real_t ground_t = -ray.origin[1] / ray.direction[1];
	if (ground_t > 0 && ground_t < closest_t) {
		closest_t = ground_t;

		rvec3_copy_scalar(dst_col, 1);
	}

	sphere_t sphere;
	sphere_intersect_t intersect;

	rvec3_copy(sphere.origin, (rvec3_t){0, 0, 0});
	sphere.radius = REAL(0.5);

	if (sphere_ray_intersect(sphere, ray, &intersect)) {
		if (intersect.distance < closest_t) {
			closest_t = intersect.distance;

			rvec3_copy(dst_col, intersect.normal);

			// Do VERY basic lambert shading
			rvec3_t sun = {1, 1, 1};
			rvec3_normalize(sun);

			real_t s = rvec3_dot(intersect.normal, sun);
			rvec3_copy_scalar(dst_col, s);
		}
	}

	//rvec3_copy(dst_col, ray.direction);
	rvec3_saturate(dst_col);
}