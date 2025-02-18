#ifndef SPHERE_H
#define SPHERE_H

#include "hittable.h"

class sphere : public hittable
{
  public:
	  sphere(const point3& center, double radius, shared_ptr<material> mat)
		  : center(center), radius(std::fmax(0, radius)), mat(mat) {}
	
	bool hit(const ray& ray, interval ray_bounds, hit_record& rec) const override
	{
		vec3 oc = center - ray.origin();
		double a = ray.direction().length_squared();
		double b = -2.0 * dot(ray.direction(), oc);
		double c = oc.length_squared() - radius * radius;
		auto hit_occured =  solve_quadratic(ray, ray_bounds, rec, a, b, c);
		if (hit_occured)
		{
			rec.p = ray.at(rec.t);
			vec3 outward_normal = (rec.p - center) / radius;
			rec.set_face_normal(ray, outward_normal);
			rec.mat = mat;
		}
		return hit_occured;
	}

	// Implicit volume within radius
	virtual bool volume_contains(const point3 p) const override
	{
		return (p - center).length_squared() <= radius * radius;
	}

  private:
	point3 center;
	double radius;
	shared_ptr<material> mat;
};

#endif