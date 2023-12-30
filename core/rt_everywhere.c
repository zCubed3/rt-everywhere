//
// Copyright (c) 2023 Liam R. (zCubed3)
//

#include "rt_everywhere.h"

#include <math.h>

#define SKY_COLOR RVEC3_RGB(51, 0, 255)
#define AMBIENT_COLOR RVEC3_RGB(44, 44, 44)

#define CAMERA_FOV 45
#define CAMERA_NEAR REAL(0.001)
#define CAMERA_FAR REAL(1000.0)

#define RTE_SIMPLE_SCENE

#ifndef RTE_SIMPLE_SCENE
#define SPHERE_COUNT 64
#define SPHERE_SIZE_MIN REAL(0.001)
#define SPHERE_SIZE_MAX REAL(0.3)
#else
#define SPHERE_COUNT 16
#define SPHERE_SIZE_MIN REAL(0.05)
#define SPHERE_SIZE_MAX REAL(0.5)
#endif

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

        rvec3_copy(RVEC_OUT(sphere.color), (rvec3_t){red, green, blue});

        sphere.radius = crand_range(SPHERE_SIZE_MIN, SPHERE_SIZE_MAX);
        //sphere.radius = REAL(0.1);

        if (crand_range(0, 1) > 0.5) {
            sphere.type = MATERIAL_TYPE_MIRROR;
        } else {
            sphere.type = MATERIAL_TYPE_PLASTIC;
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

                rvec3_copy(RVEC_OUT(planar), other.origin);
                planar[1] = 0;

                rvec3_t vector;
                rvec3_sub(RVEC_OUT(vector), planar, point);

                real_t gap = sphere.radius + other.radius;
                real_t length = rvec3_length_sqr(vector);
                if (length < gap * gap) {
                    clear = 0;
                    break;
                }
            }

            rvec3_copy(RVEC_OUT(position), (rvec3_t){x, lift, z});
        }

        rvec3_copy(RVEC_OUT(sphere.origin), position);
        spheres[s] = sphere;
    }
}

void screen_to_viewport(rvec2_out_t dst, viewport_t viewport, point_t point) {
	// Note: When x == 0, x / width = 0, but x never hits width
	// Therefore we must add half the texel size to x to account for this
	real_t tex_x = REAL(1.0) / (real_t)viewport.width;
	real_t tex_y = REAL(1.0) / (real_t)viewport.height;

	real_t u = (real_t)point.x / (real_t)viewport.width;
	real_t v = (real_t)point.y / (real_t)viewport.height;

	RVEC_OUT_DEREF(dst)[0] = ((u + (tex_x / REAL(2.0))) * REAL(2.0)) - REAL(1.0);
	RVEC_OUT_DEREF(dst)[1] = ((v + (tex_y / REAL(2.0))) * REAL(2.0)) - REAL(1.0);
}

camera_t setup_camera(viewport_t viewport, rvec3_t position, rvec3_t rotation) {
	camera_t cam;

	cam.samples = CAMERA_SAMPLES_ONE;

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

	rvec3_copy(RVEC_OUT(cam.position), position);
	rvec3_copy(RVEC_OUT(cam.rotation), rotation);

	return cam;
}

camera_t default_camera(viewport_t viewport) {
	rvec3_t origin = {0, 1, -2};
	rvec3_t target = {0, REAL(0.0), SPHERE_Z_OFFSET};

	rvec3_t direction;
	rvec3_sub(RVEC_OUT(direction), origin, target);
	rvec3_normalize(RVEC_OUT(direction));

	real_t yaw = real_to_degrees((real_t)atan2(direction[0], direction[2]));
	real_t pitch = real_to_degrees((real_t)asin(direction[1]));

	return setup_camera(viewport, origin, (rvec3_t){pitch, -yaw, 0});
}

int trace_scene(fragment_t *p_fragment, ray_t ray) {
	int hit = 0;

	p_fragment->material_type = MATERIAL_TYPE_PLASTIC;

	// Intersect the ground
	const real_t GROUND_CHECKER_SIZE = REAL(3.0);

	real_t closest_t = CAMERA_FAR;
	real_t ground_t = -ray.origin[1] / ray.direction[1];
	if (ground_t > 0 && ground_t < closest_t) {
		closest_t = ground_t;

		// Position
		rvec3_mul_scalar(RVEC_OUT(p_fragment->position), ray.direction, ground_t);
		rvec3_add(RVEC_OUT(p_fragment->position), p_fragment->position, ray.origin);

		rvec3_copy(RVEC_OUT(p_fragment->normal), (rvec3_t){0, 1, 0});

		// Checkerboard pattern
		rvec3_t checker;
		rvec3_copy(RVEC_OUT(checker), p_fragment->position);

		checker[0] = real_floor(checker[0] * GROUND_CHECKER_SIZE);
		checker[2] = real_floor(checker[2] * GROUND_CHECKER_SIZE);

		real_t mod = real_mod(checker[0] + real_mod(checker[2], REAL(2.0)), REAL(2.0));

		rvec3_copy(RVEC_OUT(p_fragment->albedo), RVEC3_RGB(0, 0, 0));

		if (mod) {
			rvec3_copy(RVEC_OUT(p_fragment->albedo), RVEC3_RGB(255, 0, 137));
		} else {
			rvec3_copy(RVEC_OUT(p_fragment->albedo), RVEC3_RGB(5, 5, 5));
		}

		rvec3_copy(RVEC_OUT(p_fragment->glow), p_fragment->albedo);
		rvec3_mul_scalar(RVEC_OUT(p_fragment->glow), p_fragment->glow, REAL(0.5));

		// The ground is a mirror
		p_fragment->material_type = MATERIAL_TYPE_MIRROR;

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

				rvec3_copy(RVEC_OUT(p_fragment->position), intersect.point);
				rvec3_copy(RVEC_OUT(p_fragment->normal), intersect.normal);
                rvec3_copy(RVEC_OUT(p_fragment->albedo), sphere.color);
                rvec3_copy(RVEC_OUT(p_fragment->glow), RVEC3_RGB(0, 0, 0));

                p_fragment->material_type = sphere.type;

				hit = 1;
			}
		}
	}

	return hit;
}

void shade_fragment(rvec3_out_t dst_col, fragment_t fragment, ray_t ray) {
	// Clear the previous shading
	rvec3_copy(dst_col, RVEC3_RGB(0, 0, 0));

	rvec3_t light_dir = {REAL(1.0), REAL(1.0), REAL(-0.5)};
	rvec3_normalize(RVEC_OUT(light_dir));

	rvec3_t bias;
	rvec3_copy(RVEC_OUT(bias), fragment.normal);
	rvec3_mul_scalar(RVEC_OUT(bias), bias, REAL(0.001));

	// View direction
	rvec3_t view_dir;
	rvec3_mul_scalar(RVEC_OUT(view_dir), ray.direction, REAL(-1.0));

	// Shadowing
	fragment_t shadow_frag;
	ray_t shadow_ray;

	rvec3_copy(RVEC_OUT(shadow_ray.origin), fragment.position);
	rvec3_add(RVEC_OUT(shadow_ray.origin), shadow_ray.origin, bias);

	rvec3_copy(RVEC_OUT(shadow_ray.direction), light_dir);

	int shadow = !trace_scene(&shadow_frag, shadow_ray);

	//
	// Lambert shading
	//
	real_t lambert = real_saturate(rvec3_dot(fragment.normal, light_dir));
	lambert *= (real_t)shadow;

	//
	// Blinn-phong
	//
	rvec3_t halfway;
	rvec3_add(RVEC_OUT(halfway), view_dir, light_dir);
	rvec3_normalize(RVEC_OUT(halfway));

	real_t blinn_phong = real_saturate(rvec3_dot(fragment.normal, halfway));
	blinn_phong = real_pow(blinn_phong, REAL(64.0));
	blinn_phong *= (real_t)shadow;

	//
	// Ambient term
	//
	rvec3_t ambient;
	rvec3_copy(RVEC_OUT(ambient), AMBIENT_COLOR);
	rvec3_mul(RVEC_OUT(ambient), ambient, fragment.albedo);

	//
	// Final shading
	//
	real_t direct_fac = lambert;
	real_t specular_fac = blinn_phong;

	// Mirrors have no diffuse and ambient component
	// But the direct factor is specular!
	if (fragment.material_type == MATERIAL_TYPE_MIRROR) {
		direct_fac = blinn_phong;
		specular_fac = REAL(0.0);
	}

	// Matte has no specular
	if (fragment.material_type == MATERIAL_TYPE_MATTE) {
		specular_fac = REAL(0.0);
	}

	rvec3_t direct;
	rvec3_t specular;

	rvec3_copy(RVEC_OUT(direct), fragment.albedo);
	rvec3_copy(RVEC_OUT(specular), RVEC3_RGB(255, 255, 255));

	rvec3_mul_scalar(RVEC_OUT(direct), direct, direct_fac);
	rvec3_mul_scalar(RVEC_OUT(specular), specular, specular_fac);

	rvec3_add(dst_col, RVEC_OUT_DEREF(dst_col), direct);
	rvec3_add(dst_col, RVEC_OUT_DEREF(dst_col), specular);

	if (fragment.material_type != MATERIAL_TYPE_MIRROR) {
		rvec3_add(dst_col, RVEC_OUT_DEREF(dst_col), ambient);
	}

	// Add the glow
	rvec3_add(dst_col, RVEC_OUT_DEREF(dst_col), fragment.glow);
}

void shade_sky(rvec3_out_t dst_col, ray_t ray) {
    // Dot against the sky
    const rvec3_t SKY_AXIS = {0, 1, 0};

    const real_t SKY_POW = REAL(0.2);

    real_t dot = real_pow(real_saturate(rvec3_dot(ray.direction, SKY_AXIS)), SKY_POW);

    rvec3_copy(dst_col, SKY_COLOR);
    rvec3_mul_scalar(dst_col, RVEC_OUT_DEREF(dst_col), dot);
}

void trace_pixel(rvec3_out_t dst_col, camera_t camera, point_t point) {
	rvec3_copy(dst_col, (rvec3_t) {0, 0, 0});

	real_t sub_tex_x = REAL(1.0) / (real_t)camera.viewport.width;
	real_t sub_tex_y = REAL(1.0) / (real_t)camera.viewport.height;

	int samples = 1;

	if (camera.samples == CAMERA_SAMPLES_FOUR) {
		samples = 4;
	}

	// Clear color
	for (int s = 0; s < samples; s++) {
		// Set up the base ray
		// It's jittered at a subpixel level when using MSAA
		rvec2_t view_coord;
		screen_to_viewport(RVEC_OUT(view_coord), camera.viewport, point);

		if (samples == 4) {
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
		}

		ray_t ray;

		rvec3_copy(RVEC_OUT(ray.direction), (rvec3_t) {view_coord[0], -view_coord[1], 1});

#ifdef RTE_FLIP_Y
		ray.direction[1] *= -1;
#endif

		rvec4_t pre_t;
		rvec4_t post_t;

		rvec4_copy_rvec3_w(RVEC_OUT(pre_t), ray.direction, REAL(1.0));
		rmat4_mul_rvec4(RVEC_OUT(post_t), camera.mat_vp_i, pre_t);

		rvec3_copy_rvec4(RVEC_OUT(ray.direction), post_t);
		rvec3_normalize(RVEC_OUT(ray.direction));

		rvec4_copy_rvec3_w(RVEC_OUT(pre_t), (rvec3_t) {0, 0, 0}, REAL(1.0));
		rmat4_mul_rvec4(RVEC_OUT(post_t), camera.mat_v, pre_t);

		rvec3_copy_rvec4(RVEC_OUT(ray.origin), post_t);

        //
		// Base pass
		//
        rvec3_t sample;
		fragment_t base_frag;

        if (trace_scene(&base_frag, ray)) {
			shade_fragment(RVEC_OUT(sample), base_frag, ray);

			// Reflection
			rvec3_t reflection;
            rvec3_copy(RVEC_OUT(reflection), RVEC3_RGB(0, 0, 0));

			if (base_frag.material_type == MATERIAL_TYPE_MIRROR) {
                const int MIRROR_BOUNCES = 5;

                fragment_t prior_frag = base_frag;
                ray_t prior_ray = ray;

                rvec3_t energy;

                rvec3_copy(&energy, base_frag.albedo);

                for (int b = 0; b < MIRROR_BOUNCES; b++) {
                    rvec3_t bias;
                    rvec3_copy(RVEC_OUT(bias), prior_frag.normal);
                    rvec3_mul_scalar(RVEC_OUT(bias), bias, REAL(0.001));

                    fragment_t reflect_frag;
                    ray_t reflect_ray;

                    rvec3_copy(RVEC_OUT(reflect_ray.origin), prior_frag.position);
                    rvec3_add(RVEC_OUT(reflect_ray.origin), reflect_ray.origin, bias);

                    rvec3_t view_dir;
                    rvec3_copy(RVEC_OUT(view_dir), prior_ray.direction);

                    rvec3_t incidence;
                    rvec3_reflect(RVEC_OUT(incidence), view_dir, prior_frag.normal);
                    rvec3_normalize(RVEC_OUT(incidence));

                    rvec3_copy(RVEC_OUT(reflect_ray.direction), incidence);

                    int break_after = 0;

                    rvec3_t local_reflection;
                    rvec3_t local_energy;

                    if (trace_scene(&reflect_frag, reflect_ray)) {
                        shade_fragment(RVEC_OUT(local_reflection), reflect_frag, reflect_ray);
                        rvec3_copy(RVEC_OUT(local_energy), reflect_frag.albedo);
                    } else {
                        shade_sky(RVEC_OUT(local_reflection), reflect_ray);
                        rvec3_copy_scalar(RVEC_OUT(local_energy), REAL(0.0));

                        break_after = 1;
                    }

                    rvec3_mul(RVEC_OUT(local_reflection), local_reflection, energy);
                    rvec3_add(RVEC_OUT(reflection), reflection, local_reflection);

                    rvec3_mul(RVEC_OUT(energy), energy, local_energy);

                    prior_frag = reflect_frag;
                    prior_ray = reflect_ray;

                    if (break_after) {
                        break;
                    }
                }
			}

			rvec3_add(RVEC_OUT(sample), sample, reflection);
		} else {
            shade_sky(RVEC_OUT(sample), ray);
        }

		rvec3_mul_scalar(RVEC_OUT(sample), sample, REAL(1.0) / (real_t)samples);
		rvec3_add(dst_col, RVEC_OUT_DEREF(dst_col), sample);
	}

	//
	// Final pass
	//
	rvec3_saturate(dst_col);
}