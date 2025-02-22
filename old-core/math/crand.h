//
// Copyright (c) 2023 Liam R. (zCubed3)
//

#ifndef RTEVERYWHERE_CRAND_H
#define RTEVERYWHERE_CRAND_H

#include "real.h"

#define CRAND_MAX 2147483647

//
// CRAND = Custom rand
//

// https://stackoverflow.com/questions/47191747/generating-random-numbers-without-using-cstdlib
extern void crand_seed(unsigned long seed);

extern long crand_next();

extern real_t crand_range(real_t min, real_t max);

#endif //RTEVERYWHERE_CRAND_H
