//
// Copyright (c) 2023 Liam R. (zCubed3)
//

#ifndef RTEVERYWHERE_MATRICES_H
#define RTEVERYWHERE_MATRICES_H

#include "real.h"
#include "vectors.h"

// ============
//  4x4 Matrix
// ============
typedef real_t rmat4_t[4][4];

extern void rmat4_zero(rmat4_t dst);
extern void rmat4_identity(rmat4_t dst);

extern void rmat4_transpose(rmat4_t dst, const rmat4_t src);
extern void rmat4_inverse(rmat4_t dst, const rmat4_t src);

extern void rmat4_rotate_x(rmat4_t dst, real_t angle);
extern void rmat4_rotate_y(rmat4_t dst, real_t angle);
extern void rmat4_rotate_z(rmat4_t dst, real_t angle);

extern void rmat4_rotate(rmat4_t dst, const rvec3_t angle);
extern void rmat4_translate(rmat4_t dst, const rvec3_t translation);

extern void rmat4_perspective(rmat4_t dst, real_t fov_y, real_t aspect, real_t near, real_t far);

extern void rmat4_mul_rvec4(rvec4_out_t dst, const rmat4_t mat, const rvec4_t vec);

extern void rmat4_mul(rmat4_t dst, const rmat4_t a, const rmat4_t b);
extern void rmat4_mul_scalar(rmat4_t dst, const rmat4_t src, real_t s);

extern void rmat4_copy(rmat4_t dst, const rmat4_t src);
extern void rmat4_copy_rows(rmat4_t dst, const rvec4_t r0, const rvec4_t r1, const rvec4_t r2, const rvec4_t r3);

// ============
//  3x3 Matrix
// ============
typedef real_t rmat3_t[3][3];

extern void rmat3_transpose(rmat3_t dst, const rmat3_t src);

extern void rmat3_mul_rvec3(rvec3_out_t dst, const rmat3_t mat, const rvec3_t vec);

#endif //RTEVERYWHERE_MATRICES_H
