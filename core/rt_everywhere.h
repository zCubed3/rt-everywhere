//
// Copyright (c) 2023 Liam R. (zCubed3)
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

typedef struct viewport {
	unsigned int width;
	unsigned int height;
} viewport_t;

typedef struct point {
	unsigned int x;
	unsigned int y;
} point_t;

typedef enum CAMERA_SAMPLES {
	CAMERA_SAMPLES_ONE,
	CAMERA_SAMPLES_FOUR
} CAMERA_SAMPLES_E;

typedef struct camera {
	viewport_t viewport;
	rvec3_t position;
	rvec3_t rotation;

	rmat4_t mat_v;
	rmat4_t mat_p;
	rmat4_t mat_vp_i;

	CAMERA_SAMPLES_E samples;
} camera_t;

typedef enum MATERIAL_TYPE {
	MATERIAL_TYPE_PLASTIC,
	MATERIAL_TYPE_MATTE,
	MATERIAL_TYPE_MIRROR
} MATERIAL_TYPE_E;

typedef struct fragment {
	rvec3_t position;
	rvec3_t normal;
	rvec3_t albedo;
    rvec3_t glow;

	real_t roughness;
	real_t metallic;

	MATERIAL_TYPE_E material_type;
} fragment_t;

extern void screen_to_viewport(rvec2_out_t dst, viewport_t viewport, point_t point);

extern camera_t setup_camera(viewport_t viewport, rvec3_t position, rvec3_t rotation);
extern camera_t default_camera(viewport_t viewport);

extern int trace_scene(fragment_t *p_fragment, ray_t ray);
extern void shade_fragment(rvec3_out_t dst_col, fragment_t fragment, ray_t ray);

extern void trace_pixel(rvec3_out_t dst_col, camera_t camera, point_t point);

#ifdef __cplusplus
};
#endif

#endif
