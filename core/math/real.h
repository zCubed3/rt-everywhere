//
// Copyright (c) 2023 Liam R. (zCubed3)
//

#ifndef RTEVERYWHERE_REAL_H
#define RTEVERYWHERE_REAL_H

#ifndef REAL_IS_DOUBLE

//typedef float real_t;
#define real_t float

#define REAL(N) N##F

#else

//typedef double real_t;
#define real_t double

#define REAL(N) N

#endif

extern real_t real_min(real_t r, real_t min);
extern real_t real_max(real_t r, real_t max);

extern real_t real_mod(real_t x, real_t y);

extern real_t real_saturate(real_t r);
extern real_t real_fract(real_t r);

extern real_t real_floor(real_t r);
extern real_t real_ceil(real_t r);

extern real_t real_pow(real_t r, real_t e);

extern real_t real_to_radians(real_t r);
extern real_t real_to_degrees(real_t r);

extern real_t real_remap(real_t r, real_t min, real_t max);

#endif //RTEVERYWHERE_REAL_H
