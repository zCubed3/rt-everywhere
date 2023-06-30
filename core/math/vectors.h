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

void rvec3_copy(rvec3_t dst, const rvec3_t src);
void rvec3_copy_scalar(rvec3_t dst, real_t s);

real_t rvec3_length(const rvec3_t vec);
real_t rvec3_dot(const rvec3_t a, const rvec3_t b);

void rvec3_normalize(rvec3_t dst);
void rvec3_saturate(rvec3_t dst);

void rvec3_add(rvec3_t dst, const rvec3_t a, const rvec3_t b);
void rvec3_sub(rvec3_t dst, const rvec3_t a, const rvec3_t b);

void rvec3_mul_scalar(rvec3_t dst, const rvec3_t a, real_t s);

#endif //RTEVERYWHERE_VECTORS_H
