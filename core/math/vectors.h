//
// Copyright (c) 2023 Liam R. (zCubed3)
//

#ifndef RTEVERYWHERE_VECTORS_H
#define RTEVERYWHERE_VECTORS_H

#include "real.h"

//#define RTE_NO_SIMD

#if __has_attribute(ext_vector_type) && !defined(RTE_NO_SIMD)

#define HANDLED_TYPES
#define VECTORS_ARE_VECTORIZED

typedef real_t rvec2_t __attribute__((ext_vector_type(2)));
typedef real_t rvec3_t __attribute__((ext_vector_type(4)));
typedef real_t rvec4_t __attribute__((ext_vector_type(4)));

#endif

#ifndef HANDLED_TYPES
typedef real_t rvec2_t[2];
typedef real_t rvec3_t[3];
typedef real_t rvec4_t[4];
#endif

#ifdef VECTORS_ARE_VECTORIZED
#define rvec2_out_t rvec2_t*
#define rvec3_out_t rvec3_t*
#define rvec4_out_t rvec4_t*

#define RVEC_OUT(VEC) (&VEC)
#define RVEC_OUT_DEREF(VEC) (*VEC)

#define RVEC_X(VEC) (VEC.x)
#define RVEC_Y(VEC) (VEC.y)
#define RVEC_Z(VEC) (VEC.z)
#define RVEC_W(VEC) (VEC.w)
#else
#define rvec2_out_t rvec2_t
#define rvec3_out_t rvec3_t
#define rvec4_out_t rvec4_t

#define RVEC_OUT(VEC) (VEC)
#define RVEC_OUT_DEREF(VEC) (VEC)

#define RVEC_X(VEC) (VEC[0])
#define RVEC_Y(VEC) (VEC[1])
#define RVEC_Z(VEC) (VEC[2])
#define RVEC_W(VEC) (VEC[3])
#endif

//
// rvec3_t
//
#define RVEC3_RGB(R, G, B) ((rvec3_t){(real_t)R / REAL(255.0), (real_t)G / REAL(255.0), (real_t)B / REAL(255.0)})

extern inline void rvec3_copy(rvec3_out_t dst, const rvec3_t src);
extern inline void rvec3_copy_scalar(rvec3_out_t dst, real_t s);
extern inline void rvec3_copy_rvec4(rvec3_out_t dst, const rvec4_t src);

extern inline real_t rvec3_length_sqr(const rvec3_t vec);
extern inline real_t rvec3_length(const rvec3_t vec);
extern inline real_t rvec3_dot(const rvec3_t a, const rvec3_t b);

extern inline void rvec3_normalize(rvec3_out_t dst);
extern inline void rvec3_saturate(rvec3_out_t dst);
extern inline void rvec3_cross(rvec3_out_t dst, const rvec3_t a, const rvec3_t b);
extern inline void rvec3_reflect(rvec3_out_t dst, const rvec3_t incoming, const rvec3_t normal);

extern inline void rvec3_add(rvec3_out_t dst, const rvec3_t a, const rvec3_t b);
extern inline void rvec3_sub(rvec3_out_t dst, const rvec3_t a, const rvec3_t b);
extern inline void rvec3_mul(rvec3_out_t dst, const rvec3_t a, const rvec3_t b);
extern inline void rvec3_div(rvec3_out_t dst, const rvec3_t a, const rvec3_t b);

extern inline void rvec3_add_scalar(rvec3_out_t dst, const rvec3_t a, real_t s);
extern inline void rvec3_sub_scalar(rvec3_out_t dst, const rvec3_t a, real_t s);
extern inline void rvec3_mul_scalar(rvec3_out_t dst, const rvec3_t a, real_t s);
extern inline void rvec3_div_scalar(rvec3_out_t dst, const rvec3_t a, real_t s);

extern inline void rvec3_rcp(rvec3_out_t dst, const rvec3_t a);

extern inline void rvec3_min_scalar(rvec3_out_t dst, const rvec3_t a, real_t s);
extern inline void rvec3_max_scalar(rvec3_out_t dst, const rvec3_t a, real_t s);

extern inline void rvec3_min(rvec3_out_t dst, const rvec3_t a, const rvec3_t b);
extern inline void rvec3_max(rvec3_out_t dst, const rvec3_t a, const rvec3_t b);

//
// rvec4_t
//
extern void rvec4_copy(rvec4_out_t dst, const rvec4_t src);
extern void rvec4_copy_rvec3(rvec4_out_t dst, const rvec3_t src);
extern void rvec4_copy_rvec3_w(rvec4_out_t dst, const rvec3_t src, real_t w);

extern void rvec4_add(rvec4_out_t dst, const rvec4_t a, const rvec4_t b);
extern void rvec4_sub(rvec4_out_t dst, const rvec4_t a, const rvec4_t b);
extern void rvec4_mul(rvec4_out_t dst, const rvec4_t a, const rvec4_t b);

extern void rvec4_mul_scalar(rvec4_out_t dst, const rvec4_t a, real_t s);

#endif //RTEVERYWHERE_VECTORS_H
