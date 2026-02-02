/*******************************************************************************************
 * File: Ray3_sse.h
 * Author: Kyle Bueche
 *
 * SoA style SIMD vector math functions with SSE, SSE2 instructions
 * In gcc, functions are only inlines when -O1 or higher is specified
 *
 ******************************************************************************************/

#ifndef RAY3_SSE_H
#define RAY3_SSE_H

#include <immintrin.h>
#include "Ray3_sse.h"

/*************************
 * SSE only instructions
 *************************/

typedef struct
{
    __m128 a;
    __m128 b;
    __m128 c;
} Quadratic_sse;

typedef struct
{
    __m128 a;
    __m128 h;
    __m128 c;
} QuadraticSub_sse;

typedef struct
{
    __m128 exists;
    __m128 t0;
    __m128 t1;
} QuadraticSolution_sse;

// Uses an h = -0.5b substitution to do less computation
inline QuadraticSolution_sse solveQuadtratic(QuadraticSub_sse quadratic)
{
    QuadraticSolution_sse solution;
    __m128 one_over_a = _mm_rcp_ps(_mm_set_ps1(a));
    __m128 h = _mm_mul_ps(b, _mm_set_ps1(-0.5f));
    __m128 inside_sqrt = _mm_sub_ps(_mm_mul_ps(h, h), _mm_mul_ps(a, c));
        
    solution.exists = _mm_cmpge_ps(inside_sqrt, _mm_setzero_ps());
    inside_sqrt = _mm_and_ps(inside_sqrt, solution.exists);
    __m128 sqrt = _mm_sqrt_ps(inside_sqrt);

    t0 = _mm_sub_ps(h, sqrt);
    t0 = _mm_mul_ps(t0, one_over_a);
    t1 = _mm_add_ps(h, sqrt);
    t1 = _mm_mul_ps(t1, one_over_a);
    solution.t0 = _mm_min_ps(t0, t1);
    solution.t1 = _mm_max_ps(t0, t1);
    return solution;
}

#endif
