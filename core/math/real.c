//
// Copyright (c) 2023 Liam R. (zCubed3)
//

#include "real.h"

// TODO: Avoid system math library for exotic platforms?
#include <math.h>

#define REAL_PI REAL(3.141592654)

real_t real_min(real_t r, real_t min) {
	return r < min ? r : min;
}

real_t real_max(real_t r, real_t max) {
	return r > max ? r : max;
}

// https://registry.khronos.org/OpenGL-Refpages/gl4/html/mod.xhtml
real_t real_mod(real_t x, real_t y) {
	return x - y * (real_t)real_floor(x / y);
}

real_t real_saturate(real_t r) {
	return real_max(REAL(0.0), real_min(REAL(1.0), r));
}

real_t real_fract(real_t r) {
	return r - (real_t)real_floor(r);
}

real_t real_floor(real_t r) {
	return (real_t)floor(r);
}

real_t real_ceil(real_t r) {
	return (real_t)ceil(r);
}

real_t real_pow(real_t r, real_t e) {
	return (real_t)pow(r, e);
}

real_t real_to_radians(real_t r) {
	return r * 	(REAL_PI / REAL(180.0));
}

real_t real_to_degrees(real_t r) {
	return r * (REAL(180.0) / REAL_PI);
}

// This assumes r is between 0 - 1
real_t real_remap(real_t r, real_t min, real_t max) {
    real_t diff = max - min;
    real_t fac = diff * r;

    return fac + min;
}