/*******************************************************************************************
 * File: simd_math.h
 * Author: Kyle Bueche
 *
 * Library for doing long streams of the same operation, vectorized and sped up using SIMD
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

#ifndef SIMD_MATH_H
#define SIMD_MATH_H

#include <immintrin.h>

// Number of each item that fits into 128 byte SIMD registers
constexpr int NUM_BYTES_IN_128 128 / 8;
constexpr int NUM_FLOATS_IN_128 128 / 32;
constexpr int NUM_DOUBLES_IN_128 128 / 64;
constexpr int NUM_INTS8_IN_128 128 / 8;
constexpr int NUM_INTS16_IN_128 128 / 16;
constexpr int NUM_INTS32_IN_128 128 / 32;
constexpr int NUM_INTS64_IN_128 128 / 64;

// Number of each item that fits into 256 byte SIMD registers
constexpr int NUM_BYTES_IN_256 256 / 8;
constexpr int NUM_FLOATS_IN_256 256 / 32;
constexpr int NUM_DOUBLES_IN_256 256 / 64;
constexpr int NUM_INTS8_IN_256 256 / 8;
constexpr int NUM_INTS16_IN_256 256 / 16;
constexpr int NUM_INTS32_IN_256 256 / 32;
constexpr int NUM_INTS64_IN_256 256 / 64;

// Number of each item that fits into 512 byte SIMD registers
constexpr int NUM_BYTES_IN_512 512 / 8;
constexpr int NUM_FLOATS_IN_512 512 / 32;
constexpr int NUM_DOUBLES_IN_512 512 / 64;
constexpr int NUM_INTS8_IN_512 512 / 8;
constexpr int NUM_INTS16_IN_512 512 / 16;
constexpr int NUM_INTS32_IN_512 512 / 32;
constexpr int NUM_INTS64_IN_512 512 / 64;


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

// SSE Add
// Behavior: out[i] = in1[i] + in2[i]
inline void SSE_add_128(float *in1, float *in2, float *out, int length)
{
    __m128 v_in1, v_in2, v_out;
    for (int i = 0; i < length; i += NUM_FLOATS_IN_128)
    {
        v_in1 = _mm_load_ps(&in1[i]);
        v_in2 = _mm_load_ps(&in2[i]);
        v_out = _mm_add_ps(v_in1, v_in2);
        _mm_store_ps(&out[i], v_out);
    }
}

// SSE Multiply
// Behavior: out[i] = in1[i] * in2[i]
inline void SSE_mul_128(float *in1, float *in2, float *out, int length)
{
    __m128 v_in1, v_in2, v_out;
    for (int i = 0; i < length; i = i + NUM_FLOATS_IN_128)
    {
        __m128 v_in1 = _mm_load_ps(&in1[i]);
        __m128 v_in2 = _mm_load_ps(&in2[i]);
        __m128 v_out = _mm_add_ps(v_in1, v_in2);
        _mm_store_ps(&out[i], v_out);
    }
}

// SSE Dot Product 3D
// Behavior: dot[i] = x1[i]*x2[i] + y1[i]*y2[i] + z1[i]*z2[i]
inline void SSE_dot3_128(float *x1, float *y1, float *z1, float *x2, float *y2, float *z2, float *dot, int length)
{
    __m128 v_x1, v_x2, v_y1, v_y2, v_z1, v_z2, v_dot;
    for (int i = 0; i < length; i = i + NUM_FLOATS_IN_128)
    {
        v_x1 = _mm_load_ps(&x1[i]);
        v_x2 = _mm_load_ps(&x2[i]);
        v_y1 = _mm_load_ps(&y1[i]);
        v_y2 = _mm_load_ps(&y2[i]);
        v_z1 = _mm_load_ps(&z1[i]);
        v_z2 = _mm_load_ps(&z2[i]);
        v_dot = _mm_mul_ps(v_x1, v_x2)
        v_dot = _mm_add_ps(_mm_mul_ps(v_y1, v_y2), v_dot);
        v_dot = _mm_add_ps(_mm_mul_ps(v_z1, v_z2), v_dot);
        _mm_store_ps(&dot[i], v_dot);
    }
}

// SSE Length Squared 3D
// Behavior: lensq[i] = x[i]^2 + y[i]^2 + z[i]^2
inline void SSE_lensq3_128(float *x, float *y, float *z, float *lensq, int length)
{
    __m128 v_x, v_y, v_z, v_lensq;
    for (int i = 0; i < length; i = i + NUM_FLOATS_IN_128)
    {
        v_x = _mm_load_ps(&x[i]);
        v_y = _mm_load_ps(&y[i]);
        v_z = _mm_load_ps(&z[i]);
        v_lensq = _mm_mul_ps(v_x, v_x)
        v_lensq = _mm_add_ps(_mm_mul_ps(v_y, v_y), v_lensq);
        v_lensq = _mm_add_ps(_mm_mul_ps(v_z, v_z), v_lensq);
        _mm_store_ps(&lensq[i], v_lensq);
    }
}

// SSE Length 3D
// Behavior: len[i] = sqrt(x[i]^2 + y[i]^2 + z[i]^2)
inline void SSE_len3_128(float *x, float *y, float *z, float *len, int length)
{
    v_x, v_y, v_z, v_len;
    for (int i = 0; i < length; i = i + NUM_FLOATS_IN_128)
    {
        v_x = _mm_load_ps(&x[i]);
        v_y = _mm_load_ps(&y[i]);
        v_z = _mm_load_ps(&z[i]);
        v_len = _mm_mul_ps(v_x, v_x)
        v_len = _mm_add_ps(_mm_mul_ps(v_y, v_y), v_len);
        v_len = _mm_add_ps(_mm_mul_ps(v_z, v_z), v_len);
        v_len = _mm_sqrt_ps(v_len);
        _mm_store_ps(&len[i], v_len);
    }
}

// SSE Solve Quadratic
// Behavior: solution_exists[i] = 0xFF if true, 0x00 if false.
SSE_solve_quadratic_128(float *a, float *b, float *c, int *solution_exists, float *t0, float *t1, float epsilon, int length)
{
    // One-time loading
    __m128 v_epsilon = _mm_load_ps1(&epsilon);
    __m128 v_one = _mm_set_ps1(1.0f);
    __m128 v_zero = _mm_setzero_ps();
    __m128 v_negative_one_half = _mm_set_ps1(-0.5f);
    __m128 v_a, v_b, v_c, v_t0, v_t1, v_h, v_inside_sqrt, v_solution_exists, v_mask, v_sqrt, v_t0_temp, v_t1_temp;
    for (int i = 0; i < length; i = i + NUM_FLOATS_IN_128)
    {
        // Loading values
        v_a = _mm_load_ps(&a[i]);
        v_b = _mm_load_ps(&b[i]);
        v_c = _mm_load_ps(&c[i]);

        // Computations
        v_one_over_a = _mm_div_ps(v_one, v_a);
        v_h = _mm_mul_ps(v_b, v_negative_one_half);
        v_inside_sqrt = _mm_sub_ps(_mm_mul_ps(v_h, v_h), _mm_mul_ps(v_a, v_c));
        
        // Mask sqrts, store boolean values for solution existence, ensure sqrts >= 0
        v_solution_exists = _mm_cmpge_ps(v_inside_sqrt, v_zero);
        v_inside_sqrt = _mm_mul_ps(v_solution_exists, v_inside_sqrt);
        v_sqrt = _mm_sqrt_ps(v_inside_sqrt);

        // Calculate solutions, t0 can potentially be greater than t1, depending on the sign of a
        v_t0_temp = _mm_sub_ps(h, v_sqrt);
        v_t0_temp = _mm_mul_ps(v_t0_temp, v_one_over_a);
        v_t1_temp = _mm_add_ps(h, v_sqrt);
        v_t1_temp = _mm_mul_ps(v_t1_temp, v_one_over_a);

        // When t0 > t1, swap them using a bitmask.
        // The mask will be all 0s or all 1s for each slot, so boolean float logic should be safe
        v_mask = _mm_cmpgt_ps(v_t0, v_t1);
        v_t0 = _mm_or_ps(_mm_andnot_ps(mask, v_t0_temp), _mm_and_ps(mask, v_t1_temp));
        v_t1 = _mm_or_ps(_mm_andnot_ps(mask, v_t1_temp), _mm_and_ps(mask, v_t0_temp));
        
        // Storing values
        // How to store solution exists? bitmask is 32 bits of 1 if true, 32 bits of 0 if false
        solution_exists, v_solution_exists);
        _mm_store_ps(&t0[i], v_t0);
        _mm_store_ps(&t1[i], v_t1);
    }
}
// SSE + FMA Dot Product 3D
// Behavior: dot[i] = x1[i]*x2[i] + y1[i]*y2[i] + z1[i]*z2[i]
inline void SSE_FMA_dot3_128(float *x1, float *y1, float *z1, float *x2, float *y2, float *z2, float *dot, int length)
{
    __m128 v_x1, v_x2, v_y1, v_y2, v_z1, v_z2, v_dot;
    for (int i = 0; i < length; i = i + NUM_FLOATS_IN_128)
    {
        v_x1 = _mm_load_ps(&x1[i]);
        v_x2 = _mm_load_ps(&x2[i]);
        v_y1 = _mm_load_ps(&y1[i]);
        v_y2 = _mm_load_ps(&y2[i]);
        v_z1 = _mm_load_ps(&z1[i]);
        v_z2 = _mm_load_ps(&z2[i]);
        v_dot = _mm_mul_ps(v_x1, v_x2)
        v_dot = _mm_fmadd_ps(v_y1, v_y2, v_dot);
        v_dot = _mm_fmadd_ps(v_z1, v_z2, v_dot);
        _mm_store_ps(&dot[i], v_dot);
    }
}

// SSE + FMA Length Squared 3D
// Behavior: lensq[i] = x[i]^2 + y[i]^2 + z[i]^2
inline void SSE_FMA_lensq3_128(float *x, float *y, float *z, float *lensq, int length)
{
    __m128 v_x, v_y, v_z, v_lensq;
    for (int i = 0; i < length; i = i + NUM_FLOATS_IN_128)
    {
        v_x = _mm_load_ps(&x[i]);
        v_y = _mm_load_ps(&y[i]);
        v_z = _mm_load_ps(&z[i]);
        v_lensq = _mm_mul_ps(v_x, v_x)
        v_lensq = _mm_fmadd_ps(v_y, v_y, v_lensq);
        v_lensq = _mm_fmadd_ps(v_z, v_z, v_lensq);
        _mm_store_ps(&lensq[i], v_lensq);
    }
}

// SSE + FMA Length 3D
// Behavior: len[i] = sqrt(x[i]^2 + y[i]^2 + z[i]^2)
inline void SSE_FMA_len3_128(float *x, float *y, float *z, float *len, int length)
{
    v_x, v_y, v_z, v_len;
    for (int i = 0; i < length; i = i + NUM_FLOATS_IN_128)
    {
        v_x = _mm_load_ps(&x[i]);
        v_y = _mm_load_ps(&y[i]);
        v_z = _mm_load_ps(&z[i]);
        v_len = _mm_mul_ps(v_x, v_x)
        v_len = _mm_fmadd_ps(v_y, v_y, v_len);
        v_len = _mm_fmadd_ps(v_z, v_z, v_len);
        v_len = _mm_sqrt_ps(v_len);
        _mm_store_ps(&len[i], v_len);
    }
}


// Double Precision Float SIMD:

/******************************************************************
 * Adds 2 64-bit doubles in parallel on a single core
 * Expects c = a + b
 * Expects contiguous 128 bytes starting from each memory address
 *
 ******************************************************************/
inline void simd_add_128(double * doubles_a, float *doubles_b, float *doubles_c)
{
    __m128d a = _mm_load_pd(doubles_a);
    __m128d b = _mm_load_pd(doubles_b);
    __m128d c = _mm_add_pd(doubles_c);
    _mm_store_pd(doubles_c, c);
}

/******************************************************************
 * Multiplies 2 64-bit doubles in parallel on a single core
 * Expects c = a * b
 * Expects contiguous 128 bytes starting from each memory address
 *
 ******************************************************************/
inline void simd_mul_128(double * doubles_a, float *doubles_b, float *doubles_c)
{
    __m128d a = _mm_load_pd(doubles_a);
    __m128d b = _mm_load_pd(doubles_b);
    __m128d c = _mm_mul_pd(doubles_c);
    _mm_store_pd(doubles_c, c);
}

#endif
