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

#include <math.h>

#define RT_EVERYWHERE_MSAA

#ifdef RT_EVERYWHERE_MSAA
#define RT_EVERYWHERE_SAMPLES 4
#else
#define RT_EVERYWHERE_SAMPLES 1
#endif

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

camera_t default_camera(viewport_t viewport) {
	rvec3_t origin = {0, 1, 2};
	rvec3_t target = {0, REAL(0.25), 0};

	rvec3_t direction;
	rvec3_sub(direction, origin, target);
	rvec3_normalize(direction);

	real_t yaw = real_to_degrees((real_t)atan2(direction[0], direction[2]));
	real_t pitch = real_to_degrees((real_t)asin(direction[1]));

	return setup_camera(viewport, origin, (rvec3_t){pitch, -yaw, 0});
}

int trace_scene(fragment_t *p_fragment, ray_t ray) {
	int hit = 0;

	p_fragment->material_type = MATERIAL_TYPE_OPAQUE;

	// Intersect the ground
	const real_t GROUND_CHECKER_SIZE = REAL(2.0);

	real_t closest_t = 100;
	real_t ground_t = -ray.origin[1] / ray.direction[1];
	if (ground_t > 0 && ground_t < closest_t) {
		closest_t = ground_t;

		// Position
		rvec3_mul_scalar(p_fragment->position, ray.direction, ground_t);
		rvec3_add(p_fragment->position, p_fragment->position, ray.origin);

		rvec3_copy(p_fragment->normal, (rvec3_t){0, 1, 0});

		// Checkerboarding
		rvec3_t checker;
		rvec3_copy(checker, p_fragment->position);

		checker[0] = real_floor(checker[0] * GROUND_CHECKER_SIZE);
		checker[2] = real_floor(checker[2] * GROUND_CHECKER_SIZE);

		real_t mod = real_mod(checker[0] + real_mod(checker[2], REAL(2.0)), REAL(2.0));

		if (mod) {
			rvec3_copy(p_fragment->color, (rvec3_t) {1, 1, 1});
		} else {
			rvec3_copy(p_fragment->color, (rvec3_t) {REAL(0.1), REAL(0.1), REAL(0.1)});
		}

		hit = 1;
	}

	sphere_t sphere;
	sphere_intersect_t intersect;

	rvec3_copy(sphere.origin, (rvec3_t){0, REAL(0.25), 0});
	sphere.radius = REAL(0.5);

	if (sphere_ray_intersect(sphere, ray, &intersect)) {
		if (intersect.distance < closest_t) {
			closest_t = intersect.distance;

			rvec3_copy(p_fragment->position, intersect.point);
			rvec3_copy(p_fragment->normal, intersect.normal);
			rvec3_copy(p_fragment->color, (rvec3_t){1, 1, 1});
			p_fragment->material_type = MATERIAL_TYPE_MIRROR;

			hit = 1;
		}
	}

	return hit;
}

void shade_fragment(rvec3_t dst_col, fragment_t fragment, ray_t ray) {
	rvec3_t light_dir = {1, REAL(0.8), REAL(0.5)};
	rvec3_normalize(light_dir);

	rvec3_t bias;
	rvec3_copy(bias, fragment.normal);
	rvec3_mul_scalar(bias, bias, REAL(0.00001));

	// Shadowing
	fragment_t shadow_frag;
	ray_t shadow_ray;

	rvec3_copy(shadow_ray.origin, fragment.position);
	rvec3_add(shadow_ray.origin, shadow_ray.origin, bias);

	rvec3_copy(shadow_ray.direction, light_dir);

	int shadow = !trace_scene(&shadow_frag, shadow_ray);

	// Lambert shading
	real_t lambert = rvec3_dot(fragment.normal, light_dir);
	lambert *= (real_t)shadow;

	rvec3_copy(dst_col, fragment.color);
	rvec3_mul_scalar(dst_col, dst_col, lambert);
}

void trace_pixel(rvec3_t dst_col, camera_t camera, point_t point) {
	rvec3_copy(dst_col, (rvec3_t) {0, 0, 0});

#ifdef RT_EVERYWHERE_MSAA
	real_t sub_tex_x = REAL(1.0) / (real_t)camera.viewport.width;
	real_t sub_tex_y = REAL(1.0) / (real_t)camera.viewport.height;
#endif

	// Clear color
	for (int s = 0; s < RT_EVERYWHERE_SAMPLES; s++) {
		rvec3_t sample;
		rvec3_copy(sample, (rvec3_t) {REAL(0.1), REAL(0.1), REAL(0.1)});

		// Set up the base ray
		// It's jittered at a subpixel level when using MSAA
		rvec2_t view_coord;
		screen_to_viewport(view_coord, camera.viewport, point);

#ifdef RT_EVERYWHERE_MSAA
		if (s <= 1) {
			view_coord[0] += sub_tex_x * REAL(0.5);
		} else {
			view_coord[0] -= sub_tex_x * REAL(0.5);
		}

		if (s % 2 == 0) {
			view_coord[1] += sub_tex_y * REAL(0.5);
		} else {
			view_coord[1] -= sub_tex_y * REAL(0.5);
		}
#endif

		ray_t ray;

		rvec3_copy(ray.direction, (rvec3_t) {view_coord[0], -view_coord[1], 1});

#ifdef RTEVERYWHERE_FLIP_Y
		ray.direction[1] *= -1;
#endif

		rvec4_t pre_t;
		rvec4_t post_t;

		rvec4_copy_rvec3_w(pre_t, ray.direction, REAL(1.0));
		rmat4_mul_rvec4(post_t, camera.mat_vp_i, pre_t);

		rvec3_copy_rvec4(ray.direction, post_t);
		rvec3_normalize(ray.direction);

		rvec4_copy_rvec3_w(pre_t, (rvec3_t) {0, 0, 0}, REAL(1.0));
		rmat4_mul_rvec4(post_t, camera.mat_v, pre_t);

		rvec3_copy_rvec4(ray.origin, post_t);

		//
		// Base pass
		//
		fragment_t base_frag;
		if (trace_scene(&base_frag, ray)) {
			shade_fragment(sample, base_frag, ray);

			// Reflection
			if (base_frag.material_type == MATERIAL_TYPE_MIRROR) {
				rvec3_t bias;
				rvec3_copy(bias, base_frag.normal);
				rvec3_mul_scalar(bias, bias, REAL(0.00001));

				fragment_t reflect_frag;
				ray_t reflect_ray;

				rvec3_copy(reflect_ray.origin, base_frag.position);
				rvec3_add(reflect_ray.origin, reflect_ray.origin, bias);

				rvec3_t view_dir;
				rvec3_copy(view_dir, ray.direction);

				rvec3_t incidence;
				rvec3_reflect(incidence, view_dir, base_frag.normal);
				rvec3_normalize(incidence);

				rvec3_copy(reflect_ray.direction, incidence);

				if (trace_scene(&reflect_frag, reflect_ray)) {
					shade_fragment(sample, reflect_frag, reflect_ray);
				} else {
					rvec3_copy(sample, (rvec3_t) {REAL(0.1), REAL(0.1), REAL(0.1)});
				}
			}
		}

		rvec3_mul_scalar(sample, sample, REAL(1.0) / (real_t)RT_EVERYWHERE_SAMPLES);
		rvec3_add(dst_col, dst_col, sample);
	}

	//
	// Final pass
	//
	rvec3_saturate(dst_col);
}