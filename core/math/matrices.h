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

#ifndef RTEVERYWHERE_MATRICES_H
#define RTEVERYWHERE_MATRICES_H

#include "real.h"
#include "vectors.h"

typedef real_t rmat4_t[4][4];

void rmat4_zero(rmat4_t dst);
void rmat4_identity(rmat4_t dst);

void rmat4_transpose(rmat4_t dst, const rmat4_t src);
void rmat4_inverse(rmat4_t dst, const rmat4_t src);

void rmat4_rotate_x(rmat4_t dst, real_t angle);
void rmat4_rotate_y(rmat4_t dst, real_t angle);
void rmat4_rotate_z(rmat4_t dst, real_t angle);

void rmat4_rotate(rmat4_t dst, const rvec3_t angle);
void rmat4_translate(rmat4_t dst, const rvec3_t translation);

void rmat4_perspective(rmat4_t dst, real_t fov_y, real_t aspect, real_t near, real_t far);

void rmat4_mul_rvec4(rvec4_t dst, const rmat4_t mat, const rvec4_t vec);

void rmat4_mul(rmat4_t dst, const rmat4_t a, const rmat4_t b);
void rmat4_mul_scalar(rmat4_t dst, const rmat4_t src, real_t s);

void rmat4_copy(rmat4_t dst, const rmat4_t src);
void rmat4_copy_rows(rmat4_t dst, const rvec4_t r0, const rvec4_t r1, const rvec4_t r2, const rvec4_t r3);

#endif //RTEVERYWHERE_MATRICES_H
