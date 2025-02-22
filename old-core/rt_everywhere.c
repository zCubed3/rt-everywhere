//
// Copyright (c) 2023 Liam R. (zCubed3)
//

#include "rt_everywhere.h"

#include <math.h>

#define SKY_COLOR RVEC3_RGB(51, 0, 255)
#define AMBIENT_COLOR RVEC3_RGB(44, 44, 44)

#define CAMERA_FOV 45
#define CAMERA_NEAR REAL(0.001)
#define CAMERA_FAR REAL(100.0)

#define TONEMAP_ACES

//#define RTE_SIMPLE_SCENE

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

void screen_to_viewport(rvec2_out_t dst, rte_viewport_t viewport, rte_point_t point) {
	// Note: When x == 0, x / width = 0, but x never hits width
	// Therefore we must add half the texel size to x to account for this
	real_t tex_x = REAL(1.0) / (real_t)viewport.width;
	real_t tex_y = REAL(1.0) / (real_t)viewport.height;

	real_t u = (real_t)point.x / (real_t)viewport.width;
	real_t v = (real_t)point.y / (real_t)viewport.height;

	RVEC_OUT_DEREF(dst)[0] = ((u + (tex_x / REAL(2.0))) * REAL(2.0)) - REAL(1.0);
	RVEC_OUT_DEREF(dst)[1] = ((v + (tex_y / REAL(2.0))) * REAL(2.0)) - REAL(1.0);
}

rte_camera_t rte_setup_camera(rte_viewport_t viewport, rvec3_t position, rvec3_t rotation) {
	rte_camera_t cam;

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

rte_camera_t rte_default_camera(rte_viewport_t viewport) {
	rvec3_t origin = {0, 1, -2};
	rvec3_t target = {0, REAL(0.0), SPHERE_Z_OFFSET};

	rvec3_t direction;
	rvec3_sub(RVEC_OUT(direction), origin, target);
	rvec3_normalize(RVEC_OUT(direction));

	real_t yaw = real_to_degrees((real_t)atan2(direction[0], direction[2]));
	real_t pitch = real_to_degrees((real_t)asin(direction[1]));

	return rte_setup_camera(viewport, origin, (rvec3_t) {pitch, -yaw, 0});
}

rte_scene_t rte_default_scene() {
    rte_scene_t scene;

    rvec3_copy(RVEC_OUT(scene.sun_light.color), RVEC3_RGB(255, 255, 255));

    rvec3_copy(RVEC_OUT(scene.sun_light.forward), (rvec3_t){REAL(0.5), REAL(1.0), REAL(-1.0)});
    rvec3_normalize(RVEC_OUT(scene.sun_light.forward));

    scene.sun_light.intensity = 1;

    scene.mirror_bounces = 3;

    return scene;
}

//#define RAYMARCHING

void tonemap_aces(rvec3_out_t color) {
    const rmat3_t m1 = {
        { REAL(0.59719), REAL(0.35458), REAL(0.04823) },
        { REAL(0.07600), REAL(0.90834), REAL(0.01566) },
        { REAL(0.02840), REAL(0.13383), REAL(0.83777) }
    };

    const rmat3_t m2 = {
        { REAL(1.60475), REAL(-0.53108), REAL(-0.07367) },
        { REAL(-0.10208), REAL(1.10813), REAL(-0.00605) },
        { REAL(-0.00327), REAL(-0.07276), REAL(1.07602) }
    };

    rvec3_t color_copy, v3;
    rvec3_copy(RVEC_OUT(color_copy), RVEC_OUT_DEREF(color));
    rvec3_mul_scalar(RVEC_OUT(color_copy), color_copy, REAL(1.8));
    rmat3_mul_rvec3(RVEC_OUT(v3), m1, color_copy);

    rvec3_t a, b;

    rvec3_add_scalar(RVEC_OUT(a), v3, REAL(0.0245786));
    rvec3_mul(RVEC_OUT(a), a, v3);
    rvec3_sub_scalar(RVEC_OUT(a), a, REAL(0.000090537));

    rvec3_mul_scalar(RVEC_OUT(b), v3, REAL(0.983729));
    rvec3_add_scalar(RVEC_OUT(b), b, REAL(0.4329510));
    rvec3_mul(RVEC_OUT(b), b, v3);
    rvec3_add_scalar(RVEC_OUT(b), b, REAL(0.238081));

    rvec3_t c;
    rvec3_div(RVEC_OUT(c), a, b);

    rvec3_copy(RVEC_OUT(v3), c);
    rmat3_mul_rvec3(color, m2, v3);
}

#ifdef RAYMARCHING

#ifdef RTE_SIMPLE_SCENE

const real_t EPSILON = REAL(0.001);
const real_t NORMAL_EPSILON = REAL(0.001);
const int MAX_STEPS = 64;

const int MAX_SHADOW_STEPS = 32;
const real_t SHADOW_SOFT = REAL(32.0);
const real_t SHADOW_EPSILON = REAL(0.000001);
const real_t SHADOW_BIAS = REAL(0.00001);

#else

const real_t EPSILON = REAL(0.001);
const real_t NORMAL_EPSILON = REAL(0.001);

const int MAX_SHADOW_STEPS = 64;
const real_t SHADOW_SOFT = REAL(64.0);
const real_t SHADOW_EPSILON = REAL(0.000001);
const real_t SHADOW_BIAS = REAL(0.00001);

#endif

inline real_t sdf_op_subtract(const real_t a, const real_t b) {
    return real_max(-a, b);
}

inline real_t sdf_op_union(const real_t a, const real_t b) {
    return real_min(a, b);
}

inline real_t sdf_sphere(const rvec3_t point, const real_t radius) {
    return rvec3_length(point) - radius;
}

inline real_t sdf_plane(const rvec3_t point, const rvec3_t normal, const real_t elevation) {
    return rvec3_dot(point, normal) + elevation;
}

real_t march_scene(const rvec3_t point) {
    const rvec3_t ground_up = {0, 1, 0};

    rvec3_t origin;
    rvec3_copy(RVEC_OUT(origin), point);
    rvec3_add(RVEC_OUT(origin), origin, (rvec3_t){0, 0, -1});

    real_t ground_sdf = sdf_plane(point, ground_up, REAL(1.0));

    real_t sphere1 = sdf_sphere(point, 1.0F);
    real_t sphere2 = sdf_sphere(origin, 1.0F);

    real_t spheres = sdf_op_subtract(sphere1, sphere2);

    return sdf_op_union(spheres, ground_sdf);
}

void march_normal(rvec3_out_t normal, const rvec3_t point) {
    rvec3_t s0, s1, s2, s3;

    rvec3_t n0 = { 1, -1, -1 };
    rvec3_t n1 = { -1, -1, 1 };
    rvec3_t n2 = { -1, 1, -1 };
    rvec3_t n3 = { 1, 1, 1 };

    rvec3_copy(RVEC_OUT(s0), point);
    rvec3_copy(RVEC_OUT(s1), point);
    rvec3_copy(RVEC_OUT(s2), point);
    rvec3_copy(RVEC_OUT(s3), point);

    rvec3_add(RVEC_OUT(s0), s0, (rvec3_t){ NORMAL_EPSILON, -NORMAL_EPSILON, -NORMAL_EPSILON });
    rvec3_add(RVEC_OUT(s1), s1, (rvec3_t){ -NORMAL_EPSILON, -NORMAL_EPSILON, NORMAL_EPSILON });
    rvec3_add(RVEC_OUT(s2), s2, (rvec3_t){ -NORMAL_EPSILON, NORMAL_EPSILON, -NORMAL_EPSILON });
    rvec3_add(RVEC_OUT(s3), s3, (rvec3_t){ NORMAL_EPSILON, NORMAL_EPSILON, NORMAL_EPSILON });

    real_t d0 = march_scene(s0);
    real_t d1 = march_scene(s1);
    real_t d2 = march_scene(s2);
    real_t d3 = march_scene(s3);

    rvec3_mul_scalar(RVEC_OUT(n0), n0, d0);
    rvec3_mul_scalar(RVEC_OUT(n1), n1, d1);
    rvec3_mul_scalar(RVEC_OUT(n2), n2, d2);
    rvec3_mul_scalar(RVEC_OUT(n3), n3, d3);

    rvec3_copy_scalar(normal, REAL(0.0));
    rvec3_add(normal, n0, n1);
    rvec3_add(normal, RVEC_OUT_DEREF(normal), n2);
    rvec3_add(normal, RVEC_OUT_DEREF(normal), n3);

    rvec3_normalize(normal);

    //return normalize(
    //    k.xyy * MarchScene(p + k.xyy * NORMAL_EPSILON).x +
    //    k.yyx * MarchScene(p + k.yyx * NORMAL_EPSILON).x +
    //    k.yxy * MarchScene(p + k.yxy * NORMAL_EPSILON).x +
    //    k.xxx * MarchScene(p + k.xxx * NORMAL_EPSILON).x
    //);
}

real_t trace_shadow(const rte_ray_t ray) {
    real_t travel = 0;
    real_t n = 1;

    rvec3_t step;
    rvec3_copy(RVEC_OUT(step), ray.origin);

    for (int s = 0; s < MAX_SHADOW_STEPS; s++) {
        real_t sdf = march_scene(step);

        if (sdf <= SHADOW_EPSILON) {
            break;
        }

        rvec3_t advance;
        rvec3_mul_scalar(RVEC_OUT(advance), ray.direction, sdf);

        rvec3_add(RVEC_OUT(step), step, advance);

        n = real_min(n, SHADOW_SOFT * sdf / travel);
        travel += sdf;
    }

    return n;
}

int trace_scene(rte_fragment_t *p_fragment, const rte_ray_t ray, const rte_scene_t scene) {
    rte_ray_t step = ray;
    int iter = 0;

    for (float d = 0; d < CAMERA_FAR || iter < MAX_STEPS;) {
        real_t sdf = march_scene(step.origin);

        if (sdf <= EPSILON) {
            rvec3_copy(RVEC_OUT(p_fragment->position), step.origin);
            march_normal(RVEC_OUT(p_fragment->normal), step.origin);

            rvec3_copy(RVEC_OUT(p_fragment->albedo), RVEC3_RGB(255, 0, 0));

            real_t heat = iter / (real_t)MAX_STEPS;
            rvec3_copy_scalar(RVEC_OUT(p_fragment->glow), heat);

            return 1;
        }

        rvec3_t advance;
        rvec3_mul_scalar(RVEC_OUT(advance), step.direction, sdf);
        rvec3_add(RVEC_OUT(step.origin), step.origin, advance);

        d += sdf;
        iter++;
    }

    return 0;
}

void shade_fragment(rvec3_out_t dst_col, const rte_fragment_t fragment, const rte_ray_t ray, const rte_scene_t scene) {
    const rvec3_t light_dir = {REAL(0.666667), REAL(0.666667), REAL(-0.333333)};

    real_t lambert = real_saturate(rvec3_dot(fragment.normal, light_dir)) * scene.sun_light.intensity;

    //rvec3_copy_scalar(dst_col, fragment.metallic / 100.0F);
    //rvec3_copy(dst_col, fragment.glow);

    rvec3_t bias;
    rvec3_mul_scalar(RVEC_OUT(bias), fragment.normal, SHADOW_BIAS);

    rte_ray_t shadow_ray;
    rvec3_add(RVEC_OUT(shadow_ray.origin), fragment.position, bias);
    rvec3_mul_scalar(RVEC_OUT(shadow_ray.direction), scene.sun_light.forward, REAL(1.0));

    real_t shadow = trace_shadow(shadow_ray);

    real_t energy = shadow * lambert;

    rvec3_mul_scalar(dst_col, fragment.albedo, energy);
    tonemap_aces(dst_col);
}

void trace_pixel(rvec3_out_t dst_col, const trace_t trace) {
    rvec3_copy(dst_col, (rvec3_t) {0, 0, 0});

    real_t sub_tex_x = REAL(1.0) / (real_t)trace.camera.viewport.width;
    real_t sub_tex_y = REAL(1.0) / (real_t)trace.camera.viewport.height;

    int samples = 1;

    if (trace.camera.samples == CAMERA_SAMPLES_FOUR) {
        samples = 4;
    }

    // Clear color
    for (int s = 0; s < samples; s++) {
        // Set up the base ray
        // It's jittered at a subpixel level when using MSAA
        rvec2_t view_coord;
        screen_to_viewport(RVEC_OUT(view_coord), trace.camera.viewport, trace.point);

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

        rte_ray_t ray;

        rvec3_copy(RVEC_OUT(ray.direction), (rvec3_t) {view_coord[0], -view_coord[1], 1});

#ifdef RTE_FLIP_Y
        ray.direction[1] *= -1;
#endif

        rvec4_t pre_t;
        rvec4_t post_t;

        rvec4_copy_rvec3_w(RVEC_OUT(pre_t), ray.direction, REAL(1.0));
        rmat4_mul_rvec4(RVEC_OUT(post_t), trace.camera.mat_vp_i, pre_t);

        rvec3_copy_rvec4(RVEC_OUT(ray.direction), post_t);
        rvec3_normalize(RVEC_OUT(ray.direction));

        rvec4_copy_rvec3_w(RVEC_OUT(pre_t), (rvec3_t) {0, 0, 0}, REAL(1.0));
        rmat4_mul_rvec4(RVEC_OUT(post_t), trace.camera.mat_v, pre_t);

        rvec3_copy_rvec4(RVEC_OUT(ray.origin), post_t);

        //
        // Base pass
        //
        rvec3_t sample;
        rte_fragment_t base_frag;

        if (trace_scene(&base_frag, ray, trace.scene)) {
            shade_fragment(RVEC_OUT(sample), base_frag, ray, trace.scene);
        } else {
            rvec3_copy(RVEC_OUT(sample), RVEC3_RGB(0, 0, 0));
        }

        rvec3_mul_scalar(RVEC_OUT(sample), sample, REAL(1.0) / (real_t)samples);
        rvec3_add(dst_col, RVEC_OUT_DEREF(dst_col), sample);
    }

    //
    // Final pass
    //
    if (trace.tonemapping == RTE_TONEMAP_ACES)
        tonemap_aces(dst_col);

    rvec3_saturate(dst_col);
}

#else

int trace_scene(rte_fragment_t *p_fragment, const rte_ray_t ray, const rte_scene_t scene) {
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

void shade_fragment(rvec3_out_t dst_col, rte_fragment_t fragment, rte_ray_t ray, const rte_scene_t scene) {
	rvec3_t bias;
	rvec3_copy(RVEC_OUT(bias), fragment.normal);
	rvec3_mul_scalar(RVEC_OUT(bias), bias, REAL(0.001));

	// View direction
	rvec3_t view_dir;
	rvec3_mul_scalar(RVEC_OUT(view_dir), ray.direction, REAL(-1.0));

	// Shadowing
	rte_fragment_t shadow_frag;
	rte_ray_t shadow_ray;

	rvec3_copy(RVEC_OUT(shadow_ray.origin), fragment.position);
	rvec3_add(RVEC_OUT(shadow_ray.origin), shadow_ray.origin, bias);

	rvec3_mul_scalar(RVEC_OUT(shadow_ray.direction), scene.sun_light.forward, REAL(1.0));

	int shadow = !trace_scene(&shadow_frag, shadow_ray, scene);

	//
	// Lambert shading
	//
	real_t lambert = real_saturate(rvec3_dot(fragment.normal, scene.sun_light.forward)) * scene.sun_light.intensity;
	lambert *= (real_t)shadow;

	//
	// Blinn-phong
	//
	rvec3_t halfway;
	rvec3_add(RVEC_OUT(halfway), view_dir, scene.sun_light.forward);
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

	rvec3_copy(dst_col, direct);
	rvec3_add(dst_col, RVEC_OUT_DEREF(dst_col), specular);

	if (fragment.material_type != MATERIAL_TYPE_MIRROR) {
		rvec3_add(dst_col, RVEC_OUT_DEREF(dst_col), ambient);
	}

	// Add the glow
	rvec3_add(dst_col, RVEC_OUT_DEREF(dst_col), fragment.glow);
}

void shade_sky(rvec3_out_t dst_col, rte_ray_t ray) {
    // Dot against the sky
    const rvec3_t SKY_AXIS = {0, 1, 0};

    const real_t SKY_POW = REAL(0.2);

    real_t dot = real_pow(real_saturate(rvec3_dot(ray.direction, SKY_AXIS)), SKY_POW);

    rvec3_copy(dst_col, SKY_COLOR);
    rvec3_mul_scalar(dst_col, RVEC_OUT_DEREF(dst_col), dot);
}

void trace_pixel(rvec3_out_t dst_col, const trace_t trace) {
	rvec3_copy(dst_col, (rvec3_t) {0, 0, 0});

	real_t sub_tex_x = REAL(1.0) / (real_t)trace.camera.viewport.width;
	real_t sub_tex_y = REAL(1.0) / (real_t)trace.camera.viewport.height;

	int samples = 1;

	if (trace.camera.samples == CAMERA_SAMPLES_FOUR) {
		samples = 4;
	}

	// Clear color
	for (int s = 0; s < samples; s++) {
		// Set up the base ray
		// It's jittered at a subpixel level when using MSAA
		rvec2_t view_coord;
		screen_to_viewport(RVEC_OUT(view_coord), trace.camera.viewport, trace.point);

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

		rte_ray_t ray;

		rvec3_copy(RVEC_OUT(ray.direction), (rvec3_t) {view_coord[0], -view_coord[1], 1});

#ifdef RTE_FLIP_Y
		ray.direction[1] *= -1;
#endif

		rvec4_t pre_t;
		rvec4_t post_t;

		rvec4_copy_rvec3_w(RVEC_OUT(pre_t), ray.direction, REAL(1.0));
		rmat4_mul_rvec4(RVEC_OUT(post_t), trace.camera.mat_vp_i, pre_t);

		rvec3_copy_rvec4(RVEC_OUT(ray.direction), post_t);
		rvec3_normalize(RVEC_OUT(ray.direction));

		rvec4_copy_rvec3_w(RVEC_OUT(pre_t), (rvec3_t) {0, 0, 0}, REAL(1.0));
		rmat4_mul_rvec4(RVEC_OUT(post_t), trace.camera.mat_v, pre_t);

		rvec3_copy_rvec4(RVEC_OUT(ray.origin), post_t);

        //
		// Base pass
		//
        rvec3_t sample;
		rte_fragment_t base_frag;

        if (trace_scene(&base_frag, ray, trace.scene)) {
			shade_fragment(RVEC_OUT(sample), base_frag, ray, trace.scene);

			// Reflection
			rvec3_t reflection;
            rvec3_copy(RVEC_OUT(reflection), RVEC3_RGB(0, 0, 0));

			if (base_frag.material_type == MATERIAL_TYPE_MIRROR) {
                rte_fragment_t prior_frag = base_frag;
                rte_ray_t prior_ray = ray;

                rvec3_t energy;

                rvec3_copy(&energy, base_frag.albedo);

                for (int b = 0; b < trace.scene.mirror_bounces; b++) {
                    rvec3_t bias;
                    rvec3_copy(RVEC_OUT(bias), prior_frag.normal);
                    rvec3_mul_scalar(RVEC_OUT(bias), bias, REAL(0.001));

                    rte_fragment_t reflect_frag;
                    rte_ray_t reflect_ray;

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

                    if (trace_scene(&reflect_frag, reflect_ray, trace.scene)) {
                        shade_fragment(RVEC_OUT(local_reflection), reflect_frag, reflect_ray, trace.scene);
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
    if (trace.tonemapping == RTE_TONEMAP_ACES)
        tonemap_aces(dst_col);

	rvec3_saturate(dst_col);
}

#endif