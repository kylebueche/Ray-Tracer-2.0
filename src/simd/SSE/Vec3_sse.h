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
    __m128 x;
    __m128 y;
    __m128 z;
} Vec3f_sse;

typedef struct
{
    __m128d x;
    __m128d y;
    __m128d z;
} Vec3d_sse;

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

inline Vec3f_sse vec3f_load_ps1(Vec3f vec)
{
    return (Vec3f_sse) {
        .x = _mm_load_ps1(vec.x);
        .y = _mm_load_ps1(vec.y);
        .z = _mm_load_ps1(vec.z);
    }
}

static __m128i rand_state_sse;

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
void __m128i sse_rand_init()
{
    rand_state_sse = _mm_set_epi32(0x12345678, 0x87654321, 0xabcdef12, 0x12fedcba);
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

inline vec3f_sse vecAdd(vec3f_sse v1, vec3f_sse v2) 
{
    vec3f_sse vec = {
        .x = _mm_add_ps(v1.x, v2.x),
        .y = _mm_add_ps(v1.y, v2.y),
        .z = _mm_add_ps(v1.z, v2.z)
    };
    return vec;
}

inline vec3f_sse vecSub(vec3f_sse v1, vec3f_sse v2) 
{
    vec3f_sse vec = {
        .x = _mm_sub_ps(v1.x, v2.x),
        .y = _mm_sub_ps(v1.y, v2.y),
        .z = _mm_sub_ps(v1.z, v2.z)
    };
    return vec;
}

inline vec3_128f vecScale(__m128 scalars, vec3f_sse v) 
{
    vec3f_sse vec = {
        .x = _mm_mul_ps(scalars, v.x),
        .y = _mm_mul_ps(scalars, v.y),
        .z = _mm_mul_ps(scalars, v.z)
    };
    return vec;
}

inline __m128 vecDot(vec3f_sse v1, vec3f_sse v2) 
{
    __m128 dot;
    dot = _mm_mul_ps(v1.x, v2.x);
    dot = _mm_add_ps(dot, _mm_mul_ps(v1.y, v2.y));
    dot = _mm_add_ps(dot, _mm_mul_ps(v1.z, v2.z));
    return dot;
}

inline vec3f_sse vecLenSq(vec3f_sse v)
{
    return vdot_128f(v, v);
}

inline vec3f_sse vecLen(vec3f_sse v)
{
    return _mm_sqrt_ps(vecLenSq(v));
}

inline vec3f_sse vecReciprocalLen(vec3f_sse v)
{
    return _mm_rsqrt_ps(vecLenSq(v));
}

inline vec3f_sse vecCross(vec3f_sse v1, vec3f_sse v2) 
{
    vec3f_sse cross = {
        .x = _mm_sub_ps(_mm_mul_ps(v1.y, v2.z), _mm_mul_ps(v1.z, v2.y));
        .y = _mm_sub_ps(_mm_mul_ps(v1.z, v2.x), _mm_mul_ps(v1.x, v2.z));
        .z = _mm_sub_ps(_mm_mul_ps(v1.x, v2.y), _mm_mul_ps(v1.y, v2.x));
    };
    return cross;
}

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


typedef struct
{
    __m128 exists;
    __m128 t;
} quadratic_solution;

// Uses an h = -0.5b substitution to do less computation
inline void solve_quadratic_128f(const __m128& a, const __m128& b, const __m128& c, __m128& solution_exists, __m128& t0, __m128& t1)
{
    quadratic_solution solution;
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
    solution.t = _mm_min_ps(t0, t1);
    return solution;
}

typedef struct
{
    vec3f_sse origin;
    vec3f_sse direction;
} Ray3f_sse;

inline void sphere_intersection_128f(Ray3f_sse rays, Sphere sphere
{
    Vec3f_sse sphereCenter = Vec3f_load_ps1(sphere.center);
    Vec3f_sse origToCenter = spherCenter - rays.origin;
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
