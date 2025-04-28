/*******************************************************************************************
 * File: simd_sse_math.h
 * Author: Kyle Bueche
 *
 * Library for doing long streams of the same operation, vectorized and sped up using SIMD
 * Optimized only for throughput. Trailing cases should be handled separately
 *
 * Intended for use in a CPU Ray Tracer, with a Structure of Arrays (SoA) architecture.
 *
 * Combining wrapper additions and multiplications into readable dot functions, etc. may
 * introduce overhead in the form of excessive loads and stores. Compiler mileage my vary.
 *
 * Therefore, this library takes specific low & high level operations that need to be done
 * a large number of times, and optimizes them individually, with readability not largely
 * taken into consideration. Learning simd intrinsics is fun, try it sometime.
 *
 ******************************************************************************************/

#ifndef SIMD_SSE_MATH_H
#define SIMD_SSE_MATH_H

#include <immintrin.h>

// Number of each item that fits into 128 byte SIMD registers
constexpr int NUM_BYTES_IN_128 128 / 8;
constexpr int NUM_FLOATS_IN_128 128 / 32;
constexpr int NUM_DOUBLES_IN_128 128 / 64;
constexpr int NUM_INTS8_IN_128 128 / 8;
constexpr int NUM_INTS16_IN_128 128 / 16;
constexpr int NUM_INTS32_IN_128 128 / 32;
constexpr int NUM_INTS64_IN_128 128 / 64;

/********************************************************
 * SSE only instructions
 * 
 * All arrays must be 16-byte aligned.
 * 
 * length is the number of float elements in the array,
 * which must be a multiple of 16 bytes
 ********************************************************/

/*********************************************************************
 * Single Precision Float Array SIMD 128:
 * 
 * Operates on four 32-bit floats at a time per input array.
 *
 * ******************************************************************/

// Functions with f(vec3, vec3) = vec3
inline void vec3_add_128f(__m128& v_x1 __m128& v_y1 __m128& v_z1, __m128& v_x2, __m128& v_y2, __m128& z2, __m128& v_x, __m128& v_y, __m128& v_z);
inline void vec3_sub_128f(__m128& v_x1 __m128& v_y1 __m128& v_z1, __m128& v_x2, __m128& v_y2, __m128& z2, __m128& v_x, __m128& v_y, __m128& v_z);
inline void vec3_cross_128f(__m128& v_x1 __m128& v_y1 __m128& v_z1, __m128& v_x2, __m128& v_y2, __m128& z2, __m128& v_x, __m128& v_y, __m128& v_z);

// Functions with f(col3, col3) = col3
inline void col3_add_128f(__m128& v_r1 __m128& v_g1 __m128& v_b1, __m128& v_r2, __m128& v_g2, __m128& b2, __m128& v_r, __m128& v_g, __m128& v_b);
inline void col3_absorb_128f(__m128& v_r1 __m128& v_g1 __m128& v_b1, __m128& v_r2, __m128& v_g2, __m128& b2, __m128& v_r, __m128& v_g, __m128& v_b);

// Functions with f(t, vec3) = vec3
inline void vec3_scale_128f(__m128& v_scalar, __m128& v_x, __m128& v_y, __m128& v_z);

// Functions with f(vec3, vec3) = t
inline void vec3_dot_128f(__m128& v_x1 __m128& v_y1 __m128& v_z1, __m128& v_x2, __m128& v_y2, __m128& z2, __m128& v_t);

// Functions with f(vec3) = t
inline void vec3_len_128f(__m128& v_x __m128& v_y __m128& v_z, __m128& v_t);
inline void vec3_len_squared_128f(__m128& v_x __m128& v_y __m128& v_z, __m128& v_t);

// Vector functions
inline void vec3_add_128f(__m128 v_x1 __m128 v_y1 __m128 v_z1, __m128 v_x2, __m128 v_y2, __m128 z2, __m128& v_x, __m128& v_y, __m128& v_z)
{
    v_x = _mm_add_ps(v_x1, v_x2);
    v_y = _mm_add_ps(v_y1, v_y2);
    v_z = _mm_add_ps(v_z1, v_z2);
}

inline void vec3_sub_128f(__m128 v_x1 __m128 v_y1 __m128 v_z1, __m128 v_x2, __m128 v_y2, __m128 z2, __m128& v_x, __m128& v_y, __m128& v_z)
{
    v_x = _mm_sub_ps(v_x1, v_x2);
    v_y = _mm_sub_ps(v_y1, v_y2);
    v_z = _mm_sub_ps(v_z1, v_z2);
}

inline void vec3_scale_128f(__m128& v_scalar, __m128& v_x, __m128& v_y, __m128& v_z)
{
    v_x = _mm_mul_ps(v_scalar, v_x);
    v_y = _mm_mul_ps(v_scalar, v_y);
    v_z = _mm_mul_ps(v_scalar, v_z);
}

inline void vec3_dot_128f(__m128& v_x1 __m128& v_y1 __m128& v_z1, __m128& v_x2, __m128& v_y2, __m128& z2, __m128& v_dot);
{
    v_dot = _mm_mul_ps(v_x1, v_x2);
    v_dot = _mm_add_ps(v_t, _mm_mul_ps(v_y1, v_y2));
    v_dot = _mm_add_ps(v_t, _mm_mul_ps(v_z1, v_z2));
}

inline void vec3_cross_128f(__m128 v_x1 __m128 v_y1 __m128 v_z1, __m128 v_x2, __m128 v_y2, __m128 z2, __m128& v_x, __m128& v_y, __m128& v_z)
{
    v_x = _mm_sub_ps(_mm_mul_ps(v_y1, v_z2), _mm_mul_ps(v_z1, v_y2));
    v_y = _mm_sub_ps(_mm_mul_ps(v_z1, v_x2), _mm_mul_ps(v_x1, v_z2));
    v_z = _mm_sub_ps(_mm_mul_ps(v_x1, v_y2), _mm_mul_ps(v_y1, v_x2));
}

inline void vec3_len_squared_128f(__m128& v_x __m128& v_y __m128& v_z, __m128& v_len_squared);
{
    v_len_squared = _mm_mul_ps(v_x, v_x);
    v_len_squared = _mm_add_ps(v_len_squared, _mm_mul_ps(v_y, v_y));
    v_len_squared = _mm_add_ps(v_len_squared, _mm_mul_ps(v_z, v_z));
}

inline void vec3_len_128f(__m128& v_x __m128& v_y __m128& v_z, __m128& v_len);
{
    v_len = _mm_mul_ps(v_x, v_x);
    v_len = _mm_add_ps(v_len, _mm_mul_ps(v_y, v_y));
    v_len = _mm_add_ps(v_len, _mm_mul_ps(v_z, v_z));
    v_len = _mm_sqrt_ps(v_len);
}

solve_quadratic_128f(__m128& v_a, __m128& v_b, __m128& v_c, __m128& v_solution_exists, __m128& v_t0, __m128& v_t1)
{
    // One-time loading
    __m128 v_one = _mm_set_ps1(1.0f);
    __m128 v_zero = _mm_setzero_ps();
    __m128 v_negative_one_half = _mm_set_ps1(-0.5f);
    __m128 v_a, v_b, v_c, v_h, v_inside_sqrt, v_solution_exists, v_mask, v_sqrt;
    // Computations
    v_one_over_a = _mm_div_ps(v_one, v_a);
    v_h = _mm_mul_ps(v_b, v_negative_one_half);
    v_inside_sqrt = _mm_sub_ps(_mm_mul_ps(v_h, v_h), _mm_mul_ps(v_a, v_c));
        
    // Mask sqrts, store boolean values for solution existence, ensure sqrts >= 0
    v_solution_exists = _mm_cmpge_ps(v_inside_sqrt, v_zero);
    v_inside_sqrt = _mm_and_ps(v_inside_sqrt, v_solution_exists); // Ensures det is zero if det is negative
    v_sqrt = _mm_sqrt_ps(v_inside_sqrt);

    // Calculate solutions, t0 can potentially be greater than t1, depending on the sign of a
    v_t0_temp = _mm_sub_ps(h, v_sqrt);
    v_t0_temp = _mm_mul_ps(v_t0_temp, v_one_over_a);
    v_t1_temp = _mm_add_ps(h, v_sqrt);
    v_t1_temp = _mm_mul_ps(v_t1_temp, v_one_over_a);
}


#endif
