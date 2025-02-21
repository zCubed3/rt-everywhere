//
// Copyright (c) 2023 Liam R. (zCubed3)
//

#include "vectors.h"

#include <math.h>

#ifndef RTE_NO_STDLIB
#include <string.h>
#endif

//
// rvec3_t
//
inline void rvec3_copy(rvec3_out_t dst, const rvec3_t src) {
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

inline void rvec3_copy_scalar(rvec3_out_t dst, real_t s) {
#if !defined(RTE_NO_STDLIB) || 1
    for (int d = 0; d < 3; d++) {
        dst[d] = s;
    }
#else
    rvec3_t src = { s, s, s };
    memcpy(dst, src, sizeof(rvec3_t));
#endif
}

inline void rvec3_copy_rvec4(rvec3_out_t dst, const rvec4_t src) {
#if !defined(RTE_NO_STDLIB) && !defined(VECTORS_ARE_VECTORIZED)
	memcpy(dst, src, sizeof(rvec3_t));
#else
	RVEC_OUT_DEREF(dst)[0] = src[0];
	RVEC_OUT_DEREF(dst)[1] = src[1];
	RVEC_OUT_DEREF(dst)[2] = src[2];
#endif
}

inline real_t rvec3_length_sqr(const rvec3_t vec) {
	return rvec3_dot(vec, vec);
}

inline real_t rvec3_length(const rvec3_t vec) {
	return real_sqrt(rvec3_dot(vec, vec));
}

inline real_t rvec3_dot(const rvec3_t a, const rvec3_t b) {
    real_t accum = REAL(0.0);

    for (int d = 0; d < 3; d++) {
        accum += a[d] * b[d];
    }

    return accum;
}

inline void rvec3_normalize(rvec3_out_t dst) {
	real_t len = REAL(1.0) / rvec3_length(RVEC_OUT_DEREF(dst));

#ifndef VECTORS_ARE_VECTORIZED
    for (int d = 0; d < 3; d++) {
        dst[d] *= len;
    }
#else
	RVEC_OUT_DEREF(dst) *= len;
#endif
}

inline void rvec3_saturate(rvec3_out_t dst) {
    for (int d = 0; d < 3; d++) {
        RVEC_OUT_DEREF(dst)[d] = real_saturate(RVEC_OUT_DEREF(dst)[d]);
    }
}

inline void rvec3_cross(rvec3_out_t dst, const rvec3_t a, const rvec3_t b) {
	RVEC_OUT_DEREF(dst)[0] = a[1] * b[2] - a[2] * b[1];
	RVEC_OUT_DEREF(dst)[1] = a[2] * b[0] - a[0] * b[2];
	RVEC_OUT_DEREF(dst)[2] = a[0] * b[1] - a[1] * b[0];
}

inline void rvec3_reflect(rvec3_out_t dst, const rvec3_t incoming, const rvec3_t normal) {
	rvec3_t fac;

	rvec3_mul_scalar(RVEC_OUT(fac), normal, rvec3_dot(incoming, normal) * REAL(2.0));
	rvec3_sub(dst, incoming, fac);
}

inline void rvec3_add(rvec3_out_t dst, const rvec3_t a, const rvec3_t b) {
#ifndef VECTORS_ARE_VECTORIZED
    for (int d = 0; d < 3; d++) {
        dst[d] = a[d] + b[d];
    }
#else
	RVEC_OUT_DEREF(dst) = a + b;
#endif
}

inline void rvec3_sub(rvec3_out_t dst, const rvec3_t a, const rvec3_t b) {
#ifndef VECTORS_ARE_VECTORIZED
    for (int d = 0; d < 3; d++) {
        dst[d] = a[d] - b[d];
    }
#else
	RVEC_OUT_DEREF(dst) = a - b;
#endif
}

inline void rvec3_mul(rvec3_out_t dst, const rvec3_t a, const rvec3_t b) {
#ifndef VECTORS_ARE_VECTORIZED
    for (int d = 0; d < 3; d++) {
        dst[d] = a[d] * b[d];
    }
#else
	RVEC_OUT_DEREF(dst) = a * b;
#endif
}

inline void rvec3_div(rvec3_out_t dst, const rvec3_t a, const rvec3_t b) {
#ifndef VECTORS_ARE_VECTORIZED
    for (int d = 0; d < 3; d++) {
        dst[d] = a[d] / b[d];
    }
#else
	RVEC_OUT_DEREF(dst) = a / b;
#endif
}

inline void rvec3_add_scalar(rvec3_out_t dst, const rvec3_t a, real_t s) {
#ifndef VECTORS_ARE_VECTORIZED
    for (int d = 0; d < 3; d++) {
        dst[d] = a[d] + s;
    }
#else
	RVEC_OUT_DEREF(dst) = a + s;
#endif
}

inline void rvec3_sub_scalar(rvec3_out_t dst, const rvec3_t a, real_t s) {
#ifndef VECTORS_ARE_VECTORIZED
    for (int d = 0; d < 3; d++) {
        dst[d] = a[d] - s;
    }
#else
	RVEC_OUT_DEREF(dst) = a - s;
#endif
}

inline void rvec3_mul_scalar(rvec3_out_t dst, const rvec3_t a, real_t s) {
#ifndef VECTORS_ARE_VECTORIZED
    for (int d = 0; d < 3; d++) {
        dst[d] = a[d] * s;
    }
#else
	RVEC_OUT_DEREF(dst) = a * s;
#endif
}

inline void rvec3_div_scalar(rvec3_out_t dst, const rvec3_t a, real_t s) {
#ifndef VECTORS_ARE_VECTORIZED
    real_t inv = 1.0 / s;

    for (int d = 0; d < 3; d++) {
        dst[d] = a[d] * inv;
    }
#else
	RVEC_OUT_DEREF(dst) = a / s;
#endif
}

inline void rvec3_rcp(rvec3_out_t dst, const rvec3_t a) {
#ifndef VECTORS_ARE_VECTORIZED
    dst[0] = REAL(1.0) / a[0];
    dst[1] = REAL(1.0) / a[1];
    dst[2] = REAL(1.0) / a[2];
#else
    RVEC_OUT_DEREF(dst) = REAL(1.0) / a;
#endif
}

inline void rvec3_min_scalar(rvec3_out_t dst, const rvec3_t a, real_t s) {
    RVEC_OUT_DEREF(dst)[0] = real_min(a[0], s);
    RVEC_OUT_DEREF(dst)[1] = real_min(a[1], s);
    RVEC_OUT_DEREF(dst)[2] = real_min(a[2], s);
}

inline void rvec3_max_scalar(rvec3_out_t dst, const rvec3_t a, real_t s) {
    RVEC_OUT_DEREF(dst)[0] = real_max(a[0], s);
    RVEC_OUT_DEREF(dst)[1] = real_max(a[1], s);
    RVEC_OUT_DEREF(dst)[2] = real_max(a[2], s);
}

inline void rvec3_min(rvec3_out_t dst, const rvec3_t a, const rvec3_t b) {
    RVEC_OUT_DEREF(dst)[0] = real_min(a[0], b[0]);
    RVEC_OUT_DEREF(dst)[1] = real_min(a[1], b[1]);
    RVEC_OUT_DEREF(dst)[2] = real_min(a[2], b[2]);
}

inline void rvec3_max(rvec3_out_t dst, const rvec3_t a, const rvec3_t b) {
    RVEC_OUT_DEREF(dst)[0] = real_max(a[0], b[0]);
    RVEC_OUT_DEREF(dst)[1] = real_max(a[1], b[1]);
    RVEC_OUT_DEREF(dst)[2] = real_max(a[2], b[2]);
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