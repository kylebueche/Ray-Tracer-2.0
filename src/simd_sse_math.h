/*******************************************************************************************
 * File: simd_sse_math.h
 * Author: Kyle Bueche
 *
 * Throughput-oriented library for doing long streams of the same operations using SIMD
 *
 * Intended for use in a CPU Ray Tracer, with a Structure of Arrays (SoA) architecture.
 *
 * In gcc, functions are only inlines when -O1 or higher is specified
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

/*************************
 * SSE only instructions
 *************************/

/********************************************
 * Single Precision Float 128
 *
 * Operates on four 32-bit floats at a time
 *
 * In the case of vec3, this means that four
 * vec3's are being worked on at once
 ********************************************/

// Random

// Need a non zero seperate seed for each slot:
class Rand128
{
    public:
        Rand128()
        {
            state = _mm_set_epi32(0x12345678, 0x87654321, 0xabcdef12, 0x12fedcba);
            mantissaShift = 32 - FLT_MANT_DIG;
            expFactor = _mm_set_ps1(1.0f / (UINT32_C(1) << FLT_MANT_DIG))l
        }
        
        __m128i nextInt()
        {
            xorshift();
            return state;
        }

        __m128 nextFloat()
        {
            xorshift();
            return _mm_mul_ps(_mm_castsi128_ps(_mm_srli_epi32(state, mantissaShift)), expFactor);
        }
        
    private:
        __m128i state;
        __m128 expFactor;
        int mantissaShift;

        void xorshift()
        {
            state = _mm_xor_si128(state, _mm_slli_epi32(state, 13));
            state = _mm_xor_si128(state, _mm_srli_epi32(state, 17));
            state = _mm_xor_si128(state, _mm_slli_epi32(state, 5));
        }
}

inline __m128i init_state_128i()
{
    return _mm_set_epi32(0x12345678, 0x87654321, 0xabcdef12, 0x12fedcba);
}

inline void xorshift_128i(__m128i& state)
{
    state = _mm_xor_si128(state, _mm_slli_epi32(state, 13));
    state = _mm_xor_si128(state, _mm_srli_epi32(state, 17));
    state = _mm_xor_si128(state, _mm_slli_epi32(state, 5));
}

// Uses a technique to map a rand32 to a float 1:1 with a range of [0;1)
// Does not generate every possible float
//
// Consider moving the sets outside for one initialization only.
inline void random_uniform_128f(__m128i& state, __m128& randomFloats)
{
    xorshift_128i(state);
    int mantissaShift = (32 - FLT_MANT_DIG);
    float exponentFactor = (1.0f / (UINT32_C(1) << FLT_MANT_DIG));
    __m128 expFactor = _mm_set_ps1(exponentFactor);
    __m128i mantissaRand = _mm_srli_epi32(state, mantissaShift);
    randomFloats = _mm_mul_ps(_mm_castsi128_ps(mantissaRand), expFactor);
}

// Math

inline void min_128f(const __m128& a, const __m128& b, __m128& min)
{
    __m128 mask = _mm_cmplt_ps(a, b);
    min = _mm_add_ps(_mm_and_ps(mask, a), _mm_andnot_ps(mask, b));
}

inline void max_128f(const __m128& a, const __m128& b, __m128& max)
{
    __m128 mask = _mm_cmpgt_ps(a, b);
    max = _mm_add_ps(_mm_and_ps(mask, a), _mm_andnot_ps(mask, b));
}

// Vector Math

inline void vec3_add_128f(const __m128 x1, const __m128 y1, const __m128 z1, const __m128 x2, const __m128 y2, const __m128 z2, __m128& x, __m128& y, __m128& z) 
{
    x = _mm_add_ps(x1, x2);
    y = _mm_add_ps(y1, y2);
    z = _mm_add_ps(z1, z2);
}

inline void vec3_sub_128f(const __m128 x1, const __m128 y1, const __m128 z1, const __m128 x2, const __m128 y2, const __m128 z2, __m128& x, __m128& y, __m128& z)
{
    x = _mm_sub_ps(x1, x2);
    y = _mm_sub_ps(y1, y2);
    z = _mm_sub_ps(z1, z2);
}

inline void vec3_scale_128f(const __m128& scalar, __m128& x, __m128& y, __m128& z)
{
    x = _mm_mul_ps(scalar, x);
    y = _mm_mul_ps(scalar, y);
    z = _mm_mul_ps(scalar, z);
}

inline void vec3_dot_128f(const __m128& x1, const __m128& y1, const __m128& z1, const __m128& x2, const __m128& y2, const __m128& z2, __m128& dot);
{
    dot = _mm_mul_ps(x1, x2);
    dot = _mm_add_ps(t, _mm_mul_ps(y1, y2));
    dot = _mm_add_ps(t, _mm_mul_ps(z1, z2));
}

inline void vec3_cross_128f(const __m128 x1, const __m128 y1, const __m128 z1, const __m128 x2, const __m128 y2, const __m128 z2, __m128& x, __m128& y, __m128& z)
{
    x = _mm_sub_ps(_mm_mul_ps(y1, z2), _mm_mul_ps(z1, y2));
    y = _mm_sub_ps(_mm_mul_ps(z1, x2), _mm_mul_ps(x1, z2));
    z = _mm_sub_ps(_mm_mul_ps(x1, y2), _mm_mul_ps(y1, x2));
}

inline void vec3_len_squared_128f(const __m128& v_x, const __m128& v_y, const __m128& v_z, __m128& v_len_squared);
{
    v_len_squared = _mm_mul_ps(v_x, v_x);
    v_len_squared = _mm_add_ps(v_len_squared, _mm_mul_ps(v_y, v_y));
    v_len_squared = _mm_add_ps(v_len_squared, _mm_mul_ps(v_z, v_z));
}

inline void vec3_len_128f(const __m128& v_x, const __m128& v_y, const __m128& v_z, __m128& v_len);
{
    v_len = _mm_mul_ps(v_x, v_x);
    v_len = _mm_add_ps(v_len, _mm_mul_ps(v_y, v_y));
    v_len = _mm_add_ps(v_len, _mm_mul_ps(v_z, v_z));
    v_len = _mm_sqrt_ps(v_len);
}

inline void col3_scale_128f(const __m128& v_scalar, __m128& v_r, __m128& v_g, __m128& v_b)
{
    v_r = _mm_mul_ps(v_scalar, v_r);
    v_g = _mm_mul_ps(v_scalar, v_g);
    v_b = _mm_mul_ps(v_scalar, v_b);
}

// This function is weird, but it's advantageous to load constants once rather than allocating each iteration
// Therefore, do:
// __m128 v_one = _mm_set_ps(1.0f);
// __m128 v_zero = _mm_setzero_ps();
// __m128 v_neg_one_half = _mm_set_ps1(-0.5f);
// outside of this function, and feed them into their respective slots if this optimization becomes worthwhile
// Uses an h = -0.5b substitution to do less computation
inline void solve_quadratic_128f(const __m128& a, const __m128& b, const __m128& c, __m128& solution_exists, __m128& t0, __m128& t1)
{
    // One-time loading
    __m128 h, mask, inside_sqrt, sqrt;
    
    // Computations
    __m128 one_over_a = _mm_div_ps(_mm_set_ps1(1.0f), a);
    h = _mm_mul_ps(b, _mm_set_ps1(-0.5f));
    inside_sqrt = _mm_sub_ps(_mm_mul_ps(h, h), _mm_mul_ps(a, c));
        
    // Mask sqrts, store boolean values for solution existence, ensure sqrts >= 0
    solution_exists = _mm_cmpge_ps(v_inside_sqrt, _mm_setzero_ps()); // Returns a mask of all 1s or all 0s per float lane
    inside_sqrt = _mm_and_ps(v_inside_sqrt, v_solution_exists); // Ensures at least 0 so no complex sqrt
    sqrt = _mm_sqrt_ps(v_inside_sqrt);

    // Calculate solutions, t0 can potentially be greater than t1, depending on the sign of a
    t0 = _mm_sub_ps(h, sqrt);
    t0 = _mm_mul_ps(t0, one_over_a);
    t1 = _mm_add_ps(h, sqrt);
    t1 = _mm_mul_ps(t1, one_over_a);
}

inline void sphere_intersection_128f(const __m128& ray_ox, const __m128& ray_oy, const __m128& ray_oz, const __m128& ray_dx, const __m128& ray_dy, const __m128& ray_dz,
        const __m128& sphere_x, __m128& sphere_y, __m128& sphere_z
                                                                                                                                                      interval ray_bounds, ) const override
{
    vec3 oc = center - ray.origin();
    double a = ray.direction().length_squared();
    double b = -2.0 * dot(ray.direction(), oc);
	double c = oc.length_squared() - radius * radius;
    bool hit_occured =  solve_quadratic(ray, ray_bounds, rec, a, b, c);
    if (hit_occured)
    {
        rec.set_face_normal(ray, (rec.p - center) / radius);
        rec.mat = mat;
    }
    return hit_occured;
}

class hit_record
{
  public:
	point3 p;
	vec3 normal;
	shared_ptr<material> mat;
	double t;
	bool front_face;

	/* Sets the hit record's normal vector, assumes normalized */
	void set_face_normal(const ray& r, const vec3& outward_normal)
	{
		front_face = dot(r.direction(), outward_normal) < 0;
		normal = front_face ? outward_normal : -outward_normal;
	}
};

#endif
