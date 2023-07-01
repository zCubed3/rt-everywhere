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

#ifndef RTEVERYWHERE_RT_EVERYWHERE_H
#define RTEVERYWHERE_RT_EVERYWHERE_H

// Here we include all our common headers
#include "math/real.h"
#include "math/vectors.h"
#include "math/matrices.h"
#include "math/ray.h"

#include "shapes/sphere.h"

typedef struct viewport {
	unsigned int width;
	unsigned int height;
} viewport_t;

typedef struct point {
	unsigned int x;
	unsigned int y;
} point_t;

typedef struct camera {
	viewport_t viewport;
	rvec3_t position;
	rvec3_t rotation;

	rmat4_t mat_v;
	rmat4_t mat_p;
	rmat4_t mat_vp_i;
} camera_t;

typedef enum MATERIAL_TYPE {
	MATERIAL_TYPE_OPAQUE,
	MATERIAL_TYPE_MIRROR
} MATERIAL_TYPE_E;

typedef struct fragment {
	rvec3_t position;
	rvec3_t normal;
	rvec3_t color;
	MATERIAL_TYPE_E material_type;
} fragment_t;

void screen_to_viewport(rvec2_t dst, viewport_t viewport, point_t point);

camera_t setup_camera(viewport_t viewport, rvec3_t position, rvec3_t rotation);
camera_t default_camera(viewport_t viewport);

int trace_scene(fragment_t *p_fragment, ray_t ray);
void shade_fragment(rvec3_t dst_col, fragment_t fragment, ray_t ray);

void trace_pixel(rvec3_t dst_col, camera_t camera, point_t point);

#endif
