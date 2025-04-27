//
// Copyright (c) 2023-2025 Liam R. (zCubed3)
//

#include "crand.h"

unsigned long state = 0xDEADBEEF;

void crand_seed(unsigned long seed) {
    state = seed;
}

long crand_next() {
    state = (state * 1103515245 + 12345) % CRAND_MAX;
    return state;
}

real_t crand_range(real_t min, real_t max) {
    long val = crand_next();

    real_t r = (real_t)val / (real_t)CRAND_MAX;
    return real_remap(r, min, max);
}