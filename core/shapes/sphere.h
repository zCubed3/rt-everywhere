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

#ifndef RTEVERYWHERE_SPHERE_H
#define RTEVERYWHERE_SPHERE_H

#include "../math/real.h"
#include "../math/vectors.h"
#include "../math/ray.h"

typedef struct sphere {
    real_t radius;
    rvec3_t origin;
} sphere_t;

typedef struct sphere_intersect {
    rvec3_t point;
    rvec3_t normal;
    real_t distance;
} sphere_intersect_t;

int sphere_ray_intersect(sphere_t sphere, ray_t ray, sphere_intersect_t* intersect);

#endif //RTEVERYWHERE_SPHERE_H
