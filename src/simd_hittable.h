#ifndef SIMD_HITTABLE
#define SIMD_HITTABLE

#include "alligned_array.h"

template <typename real>

class HittableAoSoA
{
    private:
        SphereSoA<real> spheres;
        PlaneSoA<real> planes;
        ConeSoA<real> cones;
    public:
        void addSphere(px, py, pz, radius);
        void addPlane(px, py, pz, nx, ny, nz);
        void addCone(px, py, pz, ax, ay, az, radius);
        void closest_intersections(RaySoA<real> rays, IntervalSoA<real> bounds, AllignedArray<real> intersections, MaterialSoA<real> materials);
};

class SphereSoA
{
    private:
        AllignedArray<real> px;
        AllignedArray<real> py;
        AllignedArray<real> pz;
        AllignedArray<real> radius;
    public:
        insert(real px, real py, real pz, real radius);
        remove(int index);
        void ray_intersections(RaySOA rays, float intersection);
