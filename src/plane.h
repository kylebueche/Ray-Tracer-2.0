#ifndef PLANE_H
#define PLANE_H

#include "hittable.h"

class plane : public hittable
{
public:
	plane(const point3& center, const vec3& normal, shared_ptr<material> mat)
		: center(center), normal(normal), mat(mat) {}

	bool hit(const ray& ray, interval ray_bounds, hit_record& record) const override
	{
		float denominator = dot(normal, ray.direction());
		if (denominator < 1e-6 && denominator > -1e-6)
		{
			return false;
		}

		float t = dot((center - ray.origin()), normal) / denominator;

		if (!ray_bounds.surrounds(t))
		{
			return false;
		}

		record.t = t;
		record.p = ray.at(record.t);
		record.set_face_normal(ray, normal);
		record.mat = mat;

		return true;
	}

	/* Implicit volume underneath plane */
	virtual bool volume_contains(const point3 p) const override
	{
		return dot(p - center, normal) <= 0.0;
	}

private:
	point3 center;
	vec3 normal;
	shared_ptr<material> mat;
};

#endif