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

#define SKY_COLOR RVEC3_RGB(51, 0, 255)

#define CAMERA_FOV 45
#define CAMERA_NEAR REAL(0.001)
#define CAMERA_FAR REAL(1000.0)

#define SPHERE_COUNT 64
#define SPHERE_SIZE_MIN REAL(0.001)
#define SPHERE_SIZE_MAX REAL(0.3)
#define SPHERE_Z_OFFSET REAL(2.0)

int spheres_generated = 0;
sphere_t spheres[SPHERE_COUNT];

void generate_spheres() {
    int sphere_count = sizeof(spheres) / sizeof(sphere_t);

    // Generate a batch of spheres that do not intersect
    for (int s = 0; s < sphere_count; s++) {
        sphere_t sphere;

        real_t red = crand_range(REAL(0.0), REAL(1.0));
        real_t green = crand_range(REAL(0.0), REAL(1.0));
        real_t blue = crand_range(REAL(0.0), REAL(1.0));

        rvec3_copy(sphere.color, (rvec3_t){red, green, blue});

        sphere.radius = crand_range(SPHERE_SIZE_MIN, SPHERE_SIZE_MAX);
        //sphere.radius = REAL(0.1);

        if (crand_range(0, 1) > 0.5) {
            sphere.type = MATERIAL_TYPE_MIRROR;
        } else {
            sphere.type = MATERIAL_TYPE_OPAQUE;
        }

        real_t lift = sphere.radius;

        int clear = 0;
        rvec3_t position;

        while (!clear) {
            real_t x = crand_range(REAL(-2.0), REAL(2.0));
            real_t z = crand_range(REAL(-2.0), REAL(2.0));

            z += SPHERE_Z_OFFSET;

            rvec3_t point = {x, 0, z};

            clear = 1;
            for (int p = s - 1; p >= 0; p--) {
                sphere_t other = spheres[p];

                // Planar distance
                rvec3_t planar;

                rvec3_copy(planar, other.origin);
                planar[1] = 0;

                rvec3_t vector;
                rvec3_sub(vector, planar, point);

                real_t gap = sphere.radius + other.radius;
                real_t length = rvec3_length_sqr(vector);
                if (length < gap * gap) {
                    clear = 0;
                    break;
                }
            }

            rvec3_copy(position, (rvec3_t){x, lift, z});
        }

        rvec3_copy(sphere.origin, position);
        spheres[s] = sphere;
    }
}

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
	rmat4_perspective(cam.mat_p, CAMERA_FOV, aspect, CAMERA_NEAR, CAMERA_FAR);

	rmat4_t mat_vp;

	rmat4_mul(mat_vp, cam.mat_p, cam.mat_v);
	rmat4_inverse(cam.mat_vp_i, mat_vp);

	cam.viewport = viewport;

	rvec3_copy(cam.position, position);
	rvec3_copy(cam.rotation, rotation);

	return cam;
}

camera_t default_camera(viewport_t viewport) {
	rvec3_t origin = {0, 1, -2};
	rvec3_t target = {0, REAL(0.0), SPHERE_Z_OFFSET};

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

	real_t closest_t = CAMERA_FAR;
	real_t ground_t = -ray.origin[1] / ray.direction[1];
	if (ground_t > 0 && ground_t < closest_t) {
		closest_t = ground_t;

		// Position
		rvec3_mul_scalar(p_fragment->position, ray.direction, ground_t);
		rvec3_add(p_fragment->position, p_fragment->position, ray.origin);

		rvec3_copy(p_fragment->normal, (rvec3_t){0, 1, 0});

		// Checkerboard pattern
		rvec3_t checker;
		rvec3_copy(checker, p_fragment->position);

		checker[0] = real_floor(checker[0] * GROUND_CHECKER_SIZE);
		checker[2] = real_floor(checker[2] * GROUND_CHECKER_SIZE);

		real_t mod = real_mod(checker[0] + real_mod(checker[2], REAL(2.0)), REAL(2.0));

		rvec3_copy(p_fragment->albedo, RVEC3_RGB(0, 0, 0));

		if (mod) {
			rvec3_copy(p_fragment->albedo, RVEC3_RGB(255, 0, 137));
		} else {
			rvec3_copy(p_fragment->albedo, RVEC3_RGB(5, 5, 5));
		}

		rvec3_copy(p_fragment->glow, p_fragment->albedo);
		rvec3_mul_scalar(p_fragment->glow, p_fragment->glow, REAL(0.5));

		hit = 1;
	}

	// If the spheres are not initialized, initialize then
	if (!spheres_generated) {
		generate_spheres();
        spheres_generated = 1;
	}

    int sphere_count = sizeof(spheres) / sizeof(sphere_t);
	for (int s = 0; s < sphere_count; s++) {
		sphere_t sphere = spheres[s];
		sphere_intersect_t intersect;

		if (sphere_ray_intersect(sphere, ray, &intersect)) {
			if (intersect.distance < closest_t) {
				closest_t = intersect.distance;

				rvec3_copy(p_fragment->position, intersect.point);
				rvec3_copy(p_fragment->normal, intersect.normal);
                rvec3_copy(p_fragment->albedo, sphere.color);
                rvec3_copy(p_fragment->glow, (rvec3_t){0, 0, 0});

                p_fragment->material_type = sphere.type;

				hit = 1;
			}
		}
	}

	return hit;
}

void shade_fragment(rvec3_t dst_col, fragment_t fragment, ray_t ray) {
	rvec3_t light_dir = {REAL(1.0), REAL(1.0), REAL(-0.5)};
	rvec3_normalize(light_dir);

	rvec3_t bias;
	rvec3_copy(bias, fragment.normal);
	rvec3_mul_scalar(bias, bias, REAL(0.001));

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

	rvec3_copy(dst_col, fragment.albedo);
	rvec3_mul_scalar(dst_col, dst_col, lambert);

	// Add the glow
	rvec3_add(dst_col, dst_col, fragment.glow);
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
		rvec3_copy(sample, SKY_COLOR);

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
					rvec3_copy(sample, SKY_COLOR);
				}

                rvec3_mul(sample, sample, base_frag.albedo);
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