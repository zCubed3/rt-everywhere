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
