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

#endif
