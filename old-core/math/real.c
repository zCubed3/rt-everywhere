//
// Copyright (c) 2023 Liam R. (zCubed3)
//

#include "real.h"

// TODO: Avoid system math library for exotic platforms?
#include <math.h>

#define REAL_PI REAL(3.141592654)

// =================
//  Real Arithmetic
// =================

inline real_t real_min(real_t r, real_t min) {
	return r < min ? r : min;
}

inline real_t real_max(real_t r, real_t max) {
	return r > max ? r : max;
}

// https://registry.khronos.org/OpenGL-Refpages/gl4/html/mod.xhtml
inline real_t real_mod(real_t x, real_t y) {
	return x - y * real_floor(x / y);
}

inline real_t real_saturate(real_t r) {
	return real_max(REAL(0.0), real_min(REAL(1.0), r));
}

inline real_t real_fract(real_t r) {
	return r - real_floor(r);
}

inline real_t real_floor(real_t r) {
#ifndef REAL_IS_DOUBLE
	return floorf(r);
#else
    return floor(r);
#endif
}

inline real_t real_ceil(real_t r) {
#ifndef REAL_IS_DOUBLE
    return ceilf(r);
#else
    return ceil(r);
#endif
}

inline real_t real_pow(real_t r, real_t e) {
#ifndef REAL_IS_DOUBLE
    return powf(r, e);
#else
    return pow(r, e);
#endif
}

// This assumes r is between 0 - 1
inline real_t real_remap(real_t r, real_t min, real_t max) {
    real_t diff = max - min;
    real_t fac = diff * r;

    return fac + min;
}

inline real_t real_sqrt(real_t r) {
#ifndef REAL_IS_DOUBLE
    return sqrtf(r);
#else
    return sqrt(r);
#endif
}

// ===================
//  Real Trigonometry
// ===================

inline real_t real_sin(real_t r) {
#ifndef REAL_IS_DOUBLE
    return sinf(r);
#else
    return sin(r);
#endif
}

inline real_t real_cos(real_t r) {
#ifndef REAL_IS_DOUBLE
    return cosf(r);
#else
    return cos(r);
#endif
}

inline real_t real_tan(real_t r) {
#ifndef REAL_IS_DOUBLE
    return tanf(r);
#else
    return tan(r);
#endif
}

inline real_t real_to_radians(real_t r) {
	return r * 	(REAL_PI / REAL(180.0));
}

inline real_t real_to_degrees(real_t r) {
	return r * (REAL(180.0) / REAL_PI);
}