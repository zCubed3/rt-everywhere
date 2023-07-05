/****************************************************************************************/
/* Copyright (c) 2023 zCubed3                                                        */
/*                                                                                      */
/* Permission is hereby granted, free of charge, to any person obtaining a copy         */
/* of this software and associated documentation files (the "Software"), to deal        */
/* in the Software without restriction, including without limitation the rights         */
/* to use, copy, modify, merge, publish, distribute, sublicense, and/or sell            */
/* copies of the Software, and to permit persons to whom the Software is                */
/* furnished to do so, subject to the following conditions:                             */
/*                                                                                      */
/* The above copyright notice and this permission notice shall be included in all       */
/* copies or substantial portions of the Software.                                      */
/*                                                                                      */
/* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR           */
/* IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,             */
/* FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE          */
/* AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER               */
/* LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,        */
/* OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE        */
/* SOFTWARE.                                                                            */
/****************************************************************************************/

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

real_t real_min(real_t r, real_t min);
real_t real_max(real_t r, real_t max);

real_t real_mod(real_t x, real_t y);

real_t real_saturate(real_t r);
real_t real_fract(real_t r);

real_t real_floor(real_t r);
real_t real_ceil(real_t r);

real_t real_pow(real_t r, real_t e);

real_t real_to_radians(real_t r);
real_t real_to_degrees(real_t r);

real_t real_remap(real_t r, real_t min, real_t max);

#endif //RTEVERYWHERE_REAL_H
