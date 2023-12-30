//
// Copyright (c) 2023 Liam R. (zCubed3)
//

#include "matrices.h"

#include <math.h>

#ifndef RTE_NO_STDLIB
#include <string.h>
#endif

void rmat4_zero(rmat4_t dst) {
	for (int x = 0; x < 4; x++) {
		for (int y = 0; y < 4; y++) {
			dst[x][y] = REAL(0.0);
		}
	}
}

void rmat4_identity(rmat4_t dst) {
	rmat4_zero(dst);

	dst[0][0] = REAL(1.0);
	dst[1][1] = REAL(1.0);
	dst[2][2] = REAL(1.0);
	dst[3][3] = REAL(1.0);
}

void rmat4_transpose(rmat4_t dst, const rmat4_t src) {
	for (int x = 0; x < 4; x++) {
		for (int y = 0; y < 4; y++) {
			dst[x][y] = src[y][x];
		}
	}
}

// This is ported from GLM, this is not my code!
// https://github.com/g-truc/glm/blob/master/glm/gtc/matrix_inverse.inl
void rmat4_inverse(rmat4_t dst, const rmat4_t src) {
	real_t coef00 = src[2][2] * src[3][3] - src[3][2] * src[2][3];
	real_t coef02 = src[1][2] * src[3][3] - src[3][2] * src[1][3];
	real_t coef03 = src[1][2] * src[2][3] - src[2][2] * src[1][3];

	real_t coef04 = src[2][1] * src[3][3] - src[3][1] * src[2][3];
	real_t coef06 = src[1][1] * src[3][3] - src[3][1] * src[1][3];
	real_t coef07 = src[1][1] * src[2][3] - src[2][1] * src[1][3];

	real_t coef08 = src[2][1] * src[3][2] - src[3][1] * src[2][2];
	real_t coef10 = src[1][1] * src[3][2] - src[3][1] * src[1][2];
	real_t coef11 = src[1][1] * src[2][2] - src[2][1] * src[1][2];

	real_t coef12 = src[2][0] * src[3][3] - src[3][0] * src[2][3];
	real_t coef14 = src[1][0] * src[3][3] - src[3][0] * src[1][3];
	real_t coef15 = src[1][0] * src[2][3] - src[2][0] * src[1][3];

	real_t coef16 = src[2][0] * src[3][2] - src[3][0] * src[2][2];
	real_t coef18 = src[1][0] * src[3][2] - src[3][0] * src[1][2];
	real_t coef19 = src[1][0] * src[2][2] - src[2][0] * src[1][2];

	real_t coef20 = src[2][0] * src[3][1] - src[3][0] * src[2][1];
	real_t coef22 = src[1][0] * src[3][1] - src[3][0] * src[1][1];
	real_t coef23 = src[1][0] * src[2][1] - src[2][0] * src[1][1];

	rvec4_t fac0 = {coef00, coef00, coef02, coef03};
	rvec4_t fac1 = {coef04, coef04, coef06, coef07};
	rvec4_t fac2 = {coef08, coef08, coef10, coef11};
	rvec4_t fac3 = {coef12, coef12, coef14, coef15};
	rvec4_t fac4 = {coef16, coef16, coef18, coef19};
	rvec4_t fac5 = {coef20, coef20, coef22, coef23};

	rvec4_t vec0 = {src[1][0], src[0][0], src[0][0], src[0][0]};
	rvec4_t vec1 = {src[1][1], src[0][1], src[0][1], src[0][1]};
	rvec4_t vec2 = {src[1][2], src[0][2], src[0][2], src[0][2]};
	rvec4_t vec3 = {src[1][3], src[0][3], src[0][3], src[0][3]};

	rvec4_t p00;
	rvec4_t p01;
	rvec4_t p02;
	rvec4_t p03;

	//rvec4_t inv0 = (vec1 * fac0 - vec2 * fac1 + vec3 * fac2);
	rvec4_t inv0;

	rvec4_mul(RVEC_OUT(p00), vec1, fac0);
	rvec4_mul(RVEC_OUT(p01), vec2, fac1);
	rvec4_mul(RVEC_OUT(p02), vec3, fac2);

	rvec4_add(RVEC_OUT(p03), p01, p02);
	rvec4_sub(RVEC_OUT(inv0), p00, p03);

	//rvec4_t inv1 = (vec0 * fac0 - vec2 * fac3 + vec3 * fac4);
	rvec4_t inv1;

	rvec4_mul(RVEC_OUT(p00), vec0, fac0);
	rvec4_mul(RVEC_OUT(p01), vec2, fac3);
	rvec4_mul(RVEC_OUT(p02), vec3, fac4);

	rvec4_add(RVEC_OUT(p03), p01, p02);
	rvec4_sub(RVEC_OUT(inv1), p00, p03);

	//rvec4_t inv2 = (vec0 * fac1 - vec1 * fac3 + vec3 * fac5);
	rvec4_t inv2;

	rvec4_mul(RVEC_OUT(p00), vec0, fac1);
	rvec4_mul(RVEC_OUT(p01), vec1, fac3);
	rvec4_mul(RVEC_OUT(p02), vec3, fac5);

	rvec4_add(RVEC_OUT(p03), p01, p02);
	rvec4_sub(RVEC_OUT(inv2), p00, p03);

	//rvec4_t inv3 = (vec0 * fac2 - vec1 * fac4 + vec2 * fac5);
	rvec4_t inv3;

	rvec4_mul(RVEC_OUT(p00), vec0, fac2);
	rvec4_mul(RVEC_OUT(p01), vec1, fac4);
	rvec4_mul(RVEC_OUT(p02), vec2, fac5);

	rvec4_add(RVEC_OUT(p03), p01, p02);
	rvec4_sub(RVEC_OUT(inv3), p00, p03);

	rvec4_t sign_a = {REAL(1.0), -REAL(1.0), REAL(1.0), -REAL(1.0)};
	rvec4_t sign_b;

	rvec4_mul_scalar(RVEC_OUT(sign_b), sign_a, REAL(-1.0));

	rvec4_t row0;
	rvec4_t row1;
	rvec4_t row2;
	rvec4_t row3;

	rvec4_mul(RVEC_OUT(row0), inv0, sign_a);
	rvec4_mul(RVEC_OUT(row1), inv1, sign_b);
	rvec4_mul(RVEC_OUT(row2), inv2, sign_a);
	rvec4_mul(RVEC_OUT(row3), inv3, sign_b);

	rmat4_t inv;
	rmat4_copy_rows(inv, row0, row1, row2, row3);

	rvec4_t r0 = {inv[0][0], inv[1][0], inv[2][0], inv[3][0]};
	rvec4_t src_r0 = {src[0][0], src[1][0], src[2][0], src[3][0]};

	rvec4_t dot0;
	rvec4_mul(RVEC_OUT(dot0), src_r0, r0);

	real_t dot1 = dot0[0] + dot0[1] + dot0[2] + dot0[3];
	real_t d = REAL(1.0) / dot1;

	rmat4_mul_scalar(dst, inv, d);
}

void rmat4_rotate_x(rmat4_t dst, real_t angle) {
	rmat4_identity(dst);

	dst[1][0] = 0;
	dst[1][1] = real_cos(angle);
	dst[1][2] = -real_sin(angle);
	dst[1][3] = 0;

	dst[2][0] = 0;
	dst[2][1] = real_sin(angle);
	dst[2][2] = real_cos(angle);
	dst[2][3] = 0;
}

void rmat4_rotate_y(rmat4_t dst, real_t angle) {
	rmat4_identity(dst);

	dst[0][0] = real_cos(angle);
	dst[0][1] = 0;
	dst[0][2] = real_sin(angle);
	dst[0][3] = 0;

	dst[2][0] = -real_sin(angle);
	dst[2][1] = 0;
	dst[2][2] = real_cos(angle);
	dst[2][3] = 0;
}

void rmat4_rotate_z(rmat4_t dst, real_t angle) {
	rmat4_identity(dst);

	dst[0][0] = real_cos(angle);
	dst[0][1] = -real_sin(angle);
	dst[0][2] = 0;
	dst[0][3] = 0;

	dst[1][0] = real_sin(angle);
	dst[1][1] = real_cos(angle);
	dst[1][2] = 0;
	dst[1][3] = 0;
}

void rmat4_rotate(rmat4_t dst, const rvec3_t angle) {
	rmat4_t x;
	rmat4_t y;
	rmat4_t z;

	rmat4_rotate_x(x, real_to_radians(angle[0]));
	rmat4_rotate_y(y, real_to_radians(angle[1]));
	rmat4_rotate_z(z, real_to_radians(angle[2]));

	rmat4_t combo;
	rmat4_mul(combo, x, y);
	rmat4_mul(dst, combo, z);
}

void rmat4_translate(rmat4_t dst, const rvec3_t translation) {
	rmat4_identity(dst);

	dst[0][3] = translation[0];
	dst[1][3] = translation[1];
	dst[2][3] = translation[2];
}

void rmat4_perspective(rmat4_t dst, real_t fov_y, real_t aspect, real_t near, real_t far) {
	real_t vertical_fov = real_to_radians(fov_y / REAL(2.0));
	real_t half_fov = real_tan(vertical_fov);

	rmat4_t temp;
	rmat4_zero(temp);

	temp[0][0] = REAL(1.0) / (aspect * half_fov);
	temp[1][1] = REAL(1.0) / (half_fov);
	temp[2][2] = -(far + near) / (far - near);
	temp[2][3] = -REAL(1.0);
	temp[3][2] = -(REAL(2.0) * far * near) / (far - near);

	rmat4_transpose(dst, temp);
}

void rmat4_mul_rvec4(rvec4_out_t dst, const rmat4_t mat, const rvec4_t vec) {
	RVEC_OUT_DEREF(dst)[0] = vec[0] * mat[0][0] + vec[1] * mat[0][1] + vec[2] * mat[0][2] + vec[3] * mat[0][3];
	RVEC_OUT_DEREF(dst)[1] = vec[0] * mat[1][0] + vec[1] * mat[1][1] + vec[2] * mat[1][2] + vec[3] * mat[1][3];
	RVEC_OUT_DEREF(dst)[2] = vec[0] * mat[2][0] + vec[1] * mat[2][1] + vec[2] * mat[2][2] + vec[3] * mat[2][3];
	RVEC_OUT_DEREF(dst)[3] = vec[0] * mat[3][0] + vec[1] * mat[3][1] + vec[2] * mat[3][2] + vec[3] * mat[3][3];
}

void rmat4_mul(rmat4_t dst, const rmat4_t a, const rmat4_t b) {
	for (int y = 0; y < 4; y++) {
		rvec4_t row = {a[y][0], a[y][1], a[y][2], a[y][3]};
		for (int x = 0; x < 4; x++) {
			rvec4_t col;

			for (int y2 = 0; y2 < 4; y2++) {
				col[y2] = b[y2][x];
			}

			rvec4_t prod;
			rvec4_mul(RVEC_OUT(prod), row, col);

			dst[y][x] = prod[0] + prod[1] + prod[2] + prod[3];
		}
	}
}

void rmat4_mul_scalar(rmat4_t dst, const rmat4_t src, real_t s) {
	for (int x = 0; x < 4; x++) {
		for (int y = 0; y < 4; y++) {
			dst[x][y] = src[x][y] * s;
		}
	}
}

void rmat4_copy(rmat4_t dst, const rmat4_t src) {
#ifndef RTE_NO_STDLIB
	memcpy(dst, src, sizeof(rmat4_t));
#else
	for (int x = 0; x < 4; x++) {
		for (int y = 0; y < 4; y++) {
			dst[x][y] = src[x][y];
		}
	}
#endif
}

void rmat4_copy_rows(rmat4_t dst, const rvec4_t r0, const rvec4_t r1, const rvec4_t r2, const rvec4_t r3) {
#if !defined(RTE_NO_STDLIB) && !defined(VECTORS_ARE_VECTORIZED)
	memcpy(dst[0], r0, sizeof(real_t) * 4);
	memcpy(dst[1], r1, sizeof(real_t) * 4);
	memcpy(dst[2], r2, sizeof(real_t) * 4);
	memcpy(dst[3], r3, sizeof(real_t) * 4);
#else
	for (int x = 0; x < 4; x++) {
		dst[0][x] = r0[x];
		dst[1][x] = r1[x];
		dst[2][x] = r2[x];
		dst[3][x] = r3[x];
	}
#endif
}