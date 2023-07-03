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

#ifndef RTEVERYWHERE_VECTORS_H
#define RTEVERYWHERE_VECTORS_H

#include "real.h"

typedef real_t rvec2_t[2];
typedef real_t rvec3_t[3];
typedef real_t rvec4_t[4];

//
// rvec3_t
//
#define RVEC3_RGB(R, G, B) ((rvec3_t){(real_t)R / REAL(255.0), (real_t)G / REAL(255.0), (real_t)B / REAL(255.0)})

void rvec3_copy(rvec3_t dst, const rvec3_t src);
void rvec3_copy_scalar(rvec3_t dst, real_t s);
void rvec3_copy_rvec4(rvec3_t dst, const rvec4_t src);

real_t rvec3_length_sqr(const rvec3_t vec);
real_t rvec3_length(const rvec3_t vec);
real_t rvec3_dot(const rvec3_t a, const rvec3_t b);

void rvec3_normalize(rvec3_t dst);
void rvec3_saturate(rvec3_t dst);
void rvec3_cross(rvec3_t dst, const rvec3_t a, const rvec3_t b);
void rvec3_reflect(rvec3_t dst, const rvec3_t incoming, const rvec3_t normal);

void rvec3_add(rvec3_t dst, const rvec3_t a, const rvec3_t b);
void rvec3_sub(rvec3_t dst, const rvec3_t a, const rvec3_t b);
void rvec3_mul(rvec3_t dst, const rvec3_t a, const rvec3_t b);
void rvec3_div(rvec3_t dst, const rvec3_t a, const rvec3_t b);

void rvec3_add_scalar(rvec3_t dst, const rvec3_t a, real_t s);
void rvec3_sub_scalar(rvec3_t dst, const rvec3_t a, real_t s);
void rvec3_mul_scalar(rvec3_t dst, const rvec3_t a, real_t s);
void rvec3_div_scalar(rvec3_t dst, const rvec3_t a, real_t s);

void rvec3_hsv_color(rvec3_t dst, real_t hue, real_t saturation, real_t value);

//
// rvec4_t
//
void rvec4_copy(rvec4_t dst, const rvec4_t src);
void rvec4_copy_rvec3(rvec4_t dst, const rvec3_t src);
void rvec4_copy_rvec3_w(rvec4_t dst, const rvec3_t src, real_t w);

void rvec4_add(rvec4_t dst, const rvec4_t a, const rvec4_t b);
void rvec4_sub(rvec4_t dst, const rvec4_t a, const rvec4_t b);
void rvec4_mul(rvec4_t dst, const rvec4_t a, const rvec4_t b);

void rvec4_mul_scalar(rvec4_t dst, const rvec4_t a, real_t s);

#endif //RTEVERYWHERE_VECTORS_H
