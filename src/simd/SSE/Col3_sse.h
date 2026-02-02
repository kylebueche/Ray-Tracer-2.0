/*******************************************************************************************
 * File: simd_sse_math.h
 * Author: Kyle Bueche
 *
 * SoA style SIMD vector math functions with SSE, SSE2 instructions
 * In gcc, functions are only inlines when -O1 or higher is specified
 *
 ******************************************************************************************/

#ifndef SIMD_SSE_MATH_H
#define SIMD_SSE_MATH_H

#include <immintrin.h>

/*************************
 * SSE only instructions
 *************************/

typedef struct
{
    __m128 r;
    __m128 g;
    __m128 b;
} Col3f_sse;

typedef struct
{
    __m128d r;
    __m128d g;
    __m128d b;
} Col3d_sse;

inline col3f_sse col3f_sse_scale(__m128 scalar, col3f_sse c)
{
    col3f_sse col = {
        .r = _mm_mul_ps(scalar, c.r);
        .g = _mm_mul_ps(scalar, c.g);
        .b = _mm_mul_ps(scalar, c.b);
    }
    return col;
}

inline void col3_add_128f(col3f_sse c1, col3f_sse c2)
{
    col3f_sse col = {
        .r = _mm_add_ps(c1.r, c2.r);
        .g = _mm_add_ps(c1.g, c2.g);
        .b = _mm_add_ps(c1.b, c2.b);
    };
    return col;
}

#endif
