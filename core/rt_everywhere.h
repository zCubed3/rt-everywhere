//
// Copyright (c) 2023-2025 Liam R. (zCubed3)
//

#ifndef RTEVERYWHERE_RT_EVERYWHERE_H
#define RTEVERYWHERE_RT_EVERYWHERE_H

#ifdef __cplusplus
extern "C" {
#endif

#ifdef RTE_NO_STDLIB
#warning "RTE_NO_STDLIB was defined! Some operations may become much slower!"
#endif

// Here we include all our common headers
#include "math/real.h"
#include "math/vectors.h"
#include "math/matrices.h"
#include "math/ray.h"
#include "math/crand.h"

#include "shapes/sphere.h"

typedef enum rte_bool {
    RTE_FALSE = 0,
    RTE_TRUE = 1
} rte_bool_e;

typedef struct rte_viewport {
	unsigned int width;
	unsigned int height;
} rte_viewport_t;

typedef struct rte_point {
	unsigned int x;
	unsigned int y;
} rte_point_t;

typedef enum CAMERA_SAMPLES {
	CAMERA_SAMPLES_ONE,
	CAMERA_SAMPLES_FOUR
} CAMERA_SAMPLES_E;

typedef struct rte_camera {
	rte_viewport_t viewport;
	rvec3_t position;
	rvec3_t rotation;

	rmat4_t mat_v;
	rmat4_t mat_p;
	rmat4_t mat_vp_i;

	CAMERA_SAMPLES_E samples;
} rte_camera_t;

typedef enum MATERIAL_TYPE {
	MATERIAL_TYPE_PLASTIC,
	MATERIAL_TYPE_MATTE,
	MATERIAL_TYPE_MIRROR
} MATERIAL_TYPE_E;

typedef struct rte_fragment {
	rvec3_t position;
	rvec3_t normal;
	rvec3_t albedo;
    rvec3_t glow;

	real_t roughness;
	real_t metallic;

	MATERIAL_TYPE_E material_type;
} rte_fragment_t;

typedef enum rte_tonemap {
    RTE_TONEMAP_NONE,
    RTE_TONEMAP_ACES
} rte_tonemap_e;

typedef struct rte_light {
    rvec3_t position;
    rvec3_t forward;
    rvec3_t color;
    real_t intensity;
} rte_light_t;

typedef struct rte_scene {
    rte_light_t sun_light;
    int mirror_bounces;
} rte_scene_t;

typedef struct trace {
    rte_scene_t scene;
    rte_camera_t camera;
    rte_point_t point;
    rte_tonemap_e tonemapping;
} trace_t;

extern void screen_to_viewport(rvec2_out_t dst, rte_viewport_t viewport, rte_point_t point);

extern rte_camera_t rte_setup_camera(rte_viewport_t viewport, rvec3_t position, rvec3_t rotation);
extern rte_camera_t rte_default_camera(rte_viewport_t viewport);

extern rte_scene_t rte_default_scene();

extern int trace_scene(rte_fragment_t *p_fragment, const rte_ray_t ray, const rte_scene_t scene);
extern void shade_fragment(rvec3_out_t dst_col, const rte_fragment_t fragment, const rte_ray_t ray, const rte_scene_t scene);

extern void trace_pixel(rvec3_out_t dst_col, const trace_t trace);

#ifdef __cplusplus
};
#endif

#endif
