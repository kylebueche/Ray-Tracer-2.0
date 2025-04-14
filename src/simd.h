/*******************************************************************************************
 * File: simd.h
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

#ifndef SIMD_H
#define SIMD_H

#include <immintrin.h>

// Number of each item that fits into 128 byte SIMD registers
#define NUM_BYTES_IN_128 16;
#define NUM_FLOATS_IN_128 4;
#define NUM_DOUBLES_IN_128 2;
#define NUM_INTS8_IN_128 16;
#define NUM_INTS16_IN_128 8;
#define NUM_INTS32_IN_128 4;
#define NUM_INTS64_IN_128 2;

// Number of each item that fits into 256 byte SIMD registers
#define NUM_BYTES_IN_256 32;
#define NUM_FLOATS_IN_256 8;
#define NUM_DOUBLES_IN_256 4;
#define NUM_INTS8_IN_256 32;
#define NUM_INTS16_IN_256 16;
#define NUM_INTS32_IN_256 8;
#define NUM_INTS64_IN_256 4;

/*********************************************************************
 * Single Precision Float Array SIMD 128:
 * 
 * Operates on four 32-bit floats at a time per input array.
 * Arrays must be 16-byte aligned, or 128 bits contiguous.
 * 
 * ******************************************************************/

// SIMD Add
// Behavior: out[i] = in1[i] + in2[i]
inline void simd_add_128(float *in1, float *in2, float *out, size_t offset)
{
    __m128 v_in1 = _mm_load_ps(&in1[offset]);
    __m128 v_in2 = _mm_load_ps(&in2[offset]);
    __m128 v_out = _mm_add_ps(v_in1, v_in2);
    _mm_store_ps(&out[offset], v_out);
}

// SIMD Multiply
// Behavior: out[i] = in1[i] * in2[i]
inline void simd_mul_128(float *in1, float *in2, float *out, size_t offset)
{
    __m128 v_in1 = _mm_load_ps(&in1[offset]);
    __m128 v_in2 = _mm_load_ps(&in2[offset]);
    __m128 v_out = _mm_add_ps(v_in1, v_in2);
    _mm_store_ps(&out[offset], v_out);
}

// SIMD Dot Product 3D
// Behavior: dot[i] = x1[i]*x2[i] + y1[i]*y2[i] + z1[i]*z2[i]
inline void simd_dot3_128(float *x1, float *y1, float *z1, float *x2, float *y2, float *z2, float *dot, size_t offset)
{
    __m128 v_x1 = _mm_load_ps(&x1[offset]);
    __m128 v_x2 = _mm_load_ps(&x2[offset]);
    __m128 v_y1 = _mm_load_ps(&y1[offset]);
    __m128 v_y2 = _mm_load_ps(&y2[offset]);
    __m128 v_z1 = _mm_load_ps(&z1[offset]);
    __m128 v_z2 = _mm_load_ps(&z2[offset]);
    __m128 v_dot = _mm_mul_ps(v_x1, v_x2)
    v_dot = _mm_fmadd_ps(v_y1, v_y2, v_dot);
    v_dot = _mm_fmadd_ps(v_z1, v_z2, v_dot);
    _mm_store_ps(&dot[offset], v_dot);
}

// SIMD Length Squared 3D
// Behavior: lensq[i] = x[i]^2 + y[i]^2 + z[i]^2
inline void simd_lensq3_128(float *x, float *y, float *z, float *lensq, size_t offset)
{
    __m128 v_x = _mm_load_ps(&x[offset]);
    __m128 v_y = _mm_load_ps(&y[offset]);
    __m128 v_z = _mm_load_ps(&z[offset]);
    __m128 v_lensq = _mm_mul_ps(v_x, v_x)
    v_lensq = _mm_fmadd_ps(v_y, v_y, v_lensq);
    v_lensq = _mm_fmadd_ps(v_z, v_z, v_lensq);
    _mm_store_ps(&lensq[offset], v_lensq);
}

// SIMD Length 3D
// Behavior: len[i] = sqrt(x[i]^2 + y[i]^2 + z[i]^2)
inline void simd_len3_128(float *x, float *y, float *z, float *len, size_t offset)
{
    __m128 v_x = _mm_load_ps(&x[offset]);
    __m128 v_y = _mm_load_ps(&y[offset]);
    __m128 v_z = _mm_load_ps(&z[offset]);
    __m128 v_lensq = _mm_mul_ps(v_x, v_x)
    v_len = _mm_fmadd_ps(v_y, v_y, v_len);
    v_len = _mm_fmadd_ps(v_z, v_z, v_len);
    v_len = _mm_sqrt_ps(v_len);
    _mm_store_ps(&len[offset], v_len);
}

/* Solves the quadratic equation for t given an a, b, and c, returns the first hit in the ray's bounds*/
bool solve_quadratic(float *px, float *py, float *pz, float *px, float *py, float *pz, interval ray_bounds, hit_record& record, float *a, float *b, float *c)
{
	auto h = -0.5 * b; // Simplifies the quadratic equation with substitution
	auto inside_sqrt = h * h - a * c;

	// No intersection OR infinite intersection
	if (inside_sqrt < 0.0 || (a < epsilon && a > -epsilon))
	{
		return false;
	}

	auto sqrtd = std::sqrt(inside_sqrt);

	// Ensure t0 is the closer intersection. If the denominator is negative, this will happen
	double t0 = (h - sqrtd) / a;
	double t1 = (h + sqrtd) / a;
	if (t0 > t1) std::swap(t0, t1);

	// Find the nearest root in our range.
	auto t = t0;
	if (!ray_bounds.surrounds(t))
	{
		t = t1;
		if (!ray_bounds.surrounds(t))
		{
			return false;
		}
	}
	record.t = t;
	record.p = ray.at(t);
	return true;
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
