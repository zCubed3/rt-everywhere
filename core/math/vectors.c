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

#include "vectors.h"

#include <math.h>

#ifndef RTE_NO_STDLIB
#include <string.h>
#endif

//
// rvec3_t
//
void rvec3_copy(rvec3_out_t dst, const rvec3_t src) {
#ifndef VECTORS_ARE_VECTORIZED

#if !defined(RTE_NO_STDLIB)
	memcpy(dst, src, sizeof(rvec3_t));
#else
	dst[0] = src[0];
	dst[1] = src[1];
	dst[2] = src[2];
#endif

#else
	RVEC_OUT_DEREF(dst) = src;
#endif
}

void rvec3_copy_scalar(rvec3_out_t dst, real_t s) {
	RVEC_OUT_DEREF(dst)[0] = s;
	RVEC_OUT_DEREF(dst)[1] = s;
	RVEC_OUT_DEREF(dst)[2] = s;
}

void rvec3_copy_rvec4(rvec3_out_t dst, const rvec4_t src) {
#if !defined(RTE_NO_STDLIB) && !defined(VECTORS_ARE_VECTORIZED)
	memcpy(dst, src, sizeof(rvec3_t));
#else
	RVEC_OUT_DEREF(dst)[0] = src[0];
	RVEC_OUT_DEREF(dst)[1] = src[1];
	RVEC_OUT_DEREF(dst)[2] = src[2];
#endif
}

real_t rvec3_length_sqr(const rvec3_t vec) {
	return rvec3_dot(vec, vec);
}

real_t rvec3_length(const rvec3_t vec) {
	return (real_t)sqrt(rvec3_dot(vec, vec));
}

real_t rvec3_dot(const rvec3_t a, const rvec3_t b) {
	return
	a[0] * b[0] +
	a[1] * b[1] +
	a[2] * b[2];
}

void rvec3_normalize(rvec3_out_t dst) {
	real_t len = rvec3_length(RVEC_OUT_DEREF(dst));

#ifndef VECTORS_ARE_VECTORIZED
	dst[0] /= len;
	dst[1] /= len;
	dst[2] /= len;
#else
	RVEC_OUT_DEREF(dst) /= len;
#endif
}

void rvec3_saturate(rvec3_out_t dst) {
	RVEC_OUT_DEREF(dst)[0] = real_saturate(RVEC_OUT_DEREF(dst)[0]);
	RVEC_OUT_DEREF(dst)[1] = real_saturate(RVEC_OUT_DEREF(dst)[1]);
	RVEC_OUT_DEREF(dst)[2] = real_saturate(RVEC_OUT_DEREF(dst)[2]);
}

void rvec3_cross(rvec3_out_t dst, const rvec3_t a, const rvec3_t b) {
	RVEC_OUT_DEREF(dst)[0] = a[1] * b[2] - a[2] * b[1];
	RVEC_OUT_DEREF(dst)[1] = a[2] * b[0] - a[0] * b[2];
	RVEC_OUT_DEREF(dst)[2] = a[0] * b[1] - a[1] * b[0];
}

void rvec3_reflect(rvec3_out_t dst, const rvec3_t incoming, const rvec3_t normal) {
	rvec3_t fac;
	rvec3_mul_scalar(RVEC_OUT(fac), normal, rvec3_dot(incoming, normal) * REAL(2.0));

	rvec3_sub(dst, incoming, fac);
}

void rvec3_add(rvec3_out_t dst, const rvec3_t a, const rvec3_t b) {
#ifndef VECTORS_ARE_VECTORIZED
	dst[0] = a[0] + b[0];
	dst[1] = a[1] + b[1];
	dst[2] = a[2] + b[2];
#else
	RVEC_OUT_DEREF(dst) = a + b;
#endif
}

void rvec3_sub(rvec3_out_t dst, const rvec3_t a, const rvec3_t b) {
#ifndef VECTORS_ARE_VECTORIZED
	dst[0] = a[0] - b[0];
	dst[1] = a[1] - b[1];
	dst[2] = a[2] - b[2];
#else
	RVEC_OUT_DEREF(dst) = a - b;
#endif
}

void rvec3_mul(rvec3_out_t dst, const rvec3_t a, const rvec3_t b) {
#ifndef VECTORS_ARE_VECTORIZED
	dst[0] = a[0] * b[0];
	dst[1] = a[1] * b[1];
	dst[2] = a[2] * b[2];
#else
	RVEC_OUT_DEREF(dst) = a * b;
#endif
}

void rvec3_div(rvec3_out_t dst, const rvec3_t a, const rvec3_t b) {
#ifndef VECTORS_ARE_VECTORIZED
	dst[0] = a[0] / b[0];
	dst[1] = a[1] / b[1];
	dst[2] = a[2] / b[2];
#else
	RVEC_OUT_DEREF(dst) = a / b;
#endif
}

void rvec3_add_scalar(rvec3_out_t dst, const rvec3_t a, real_t s) {
#ifndef VECTORS_ARE_VECTORIZED
	dst[0] = a[0] + s;
	dst[1] = a[1] + s;
	dst[2] = a[2] + s;
#else
	RVEC_OUT_DEREF(dst) = a + s;
#endif
}

void rvec3_sub_scalar(rvec3_out_t dst, const rvec3_t a, real_t s) {
#ifndef VECTORS_ARE_VECTORIZED
	dst[0] = a[0] - s;
	dst[1] = a[1] - s;
	dst[2] = a[2] - s;
#else
	RVEC_OUT_DEREF(dst) = a - s;
#endif
}

void rvec3_mul_scalar(rvec3_out_t dst, const rvec3_t a, real_t s) {
#ifndef VECTORS_ARE_VECTORIZED
	dst[0] = a[0] * s;
	dst[1] = a[1] * s;
	dst[2] = a[2] * s;
#else
	RVEC_OUT_DEREF(dst) = a * s;
#endif
}

void rvec3_div_scalar(rvec3_out_t dst, const rvec3_t a, real_t s) {
#ifndef VECTORS_ARE_VECTORIZED
	dst[0] = a[0] / s;
	dst[1] = a[1] / s;
	dst[2] = a[2] / s;
#else
	RVEC_OUT_DEREF(dst) = a / s;
#endif
}

//
// rvec4_t
//
void rvec4_copy(rvec4_out_t dst, const rvec4_t src) {
#ifndef VECTORS_ARE_VECTORIZED

#if !defined(RTE_NO_STDLIB)
	memcpy(dst, src, sizeof(rvec4_t));
#else
	dst[0] = src[0];
	dst[1] = src[1];
	dst[2] = src[2];
	dst[3] = src[3];
#endif

#else
	RVEC_OUT_DEREF(dst) = src;
#endif
}

void rvec4_copy_rvec3(rvec4_out_t dst, const rvec3_t src) {
#if !defined(RTE_NO_STDLIB) && !defined(VECTORS_ARE_VECTORIZED)
	memcpy(dst, src, sizeof(rvec3_t));
#else
	RVEC_OUT_DEREF(dst)[0] = src[0];
	RVEC_OUT_DEREF(dst)[1] = src[1];
	RVEC_OUT_DEREF(dst)[2] = src[2];
#endif

	RVEC_OUT_DEREF(dst)[3] = REAL(0.0);
}

void rvec4_copy_rvec3_w(rvec4_out_t dst, const rvec3_t src, real_t w) {
#if !defined(RTE_NO_STDLIB) && !defined(VECTORS_ARE_VECTORIZED)
	memcpy(dst, src, sizeof(rvec3_t));
#else
	RVEC_OUT_DEREF(dst)[0] = src[0];
	RVEC_OUT_DEREF(dst)[1] = src[1];
	RVEC_OUT_DEREF(dst)[2] = src[2];
#endif

	RVEC_OUT_DEREF(dst)[3] = w;
}

void rvec4_add(rvec4_out_t dst, const rvec4_t a, const rvec4_t b) {
#ifndef VECTORS_ARE_VECTORIZED
	dst[0] = a[0] + b[0];
	dst[1] = a[1] + b[1];
	dst[2] = a[2] + b[2];
	dst[3] = a[3] + b[3];
#else
	RVEC_OUT_DEREF(dst) = a + b;
#endif
}

void rvec4_sub(rvec4_out_t dst, const rvec4_t a, const rvec4_t b) {
#ifndef VECTORS_ARE_VECTORIZED
	dst[0] = a[0] - b[0];
	dst[1] = a[1] - b[1];
	dst[2] = a[2] - b[2];
	dst[3] = a[3] - b[3];
#else
	RVEC_OUT_DEREF(dst) = a - b;
#endif
}

void rvec4_mul(rvec4_out_t dst, const rvec4_t a, const rvec4_t b) {
#ifndef VECTORS_ARE_VECTORIZED
	dst[0] = a[0] * b[0];
	dst[1] = a[1] * b[1];
	dst[2] = a[2] * b[2];
	dst[3] = a[3] * b[3];
#else
	RVEC_OUT_DEREF(dst) = a * b;
#endif
}

void rvec4_mul_scalar(rvec4_out_t dst, const rvec4_t a, real_t s) {
#ifndef VECTORS_ARE_VECTORIZED
	dst[0] = a[0] * s;
	dst[1] = a[1] * s;
	dst[2] = a[2] * s;
	dst[3] = a[3] * s;
#else
	RVEC_OUT_DEREF(dst) = a * s;
#endif
}