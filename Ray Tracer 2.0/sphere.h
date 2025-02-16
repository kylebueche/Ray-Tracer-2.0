#ifndef SPHERE_H
#define SPHERE_H

#include "hittable.h"

class sphere : public hittable
{
  public:
	  sphere(const point3& center, double radius, shared_ptr<material> mat)
		  : center(center), radius(std::fmax(0, radius)), mat(mat) {}
	
	bool hit(const ray& r, interval ray_t, hit_record& rec) const override
	{
		vec3 oc = center - r.origin();
		auto a = r.direction().length_squared();
		auto h = dot(r.direction(), oc);
		auto c = oc.length_squared() - radius * radius;
		auto discriminant = h * h - a * c;

		if (discriminant < 0)
		{
			return false;
		}
		auto sqrtd = std::sqrt(discriminant);

		// Find the nearest root in our range.
		auto root = (h - sqrtd) / a;
		if (!ray_t.surrounds(root))
		{
			root = (h + sqrtd) / a;
			if (!ray_t.surrounds(root))
			{
				return false;
			}
		}

		rec.t = root;
		rec.p = r.at(rec.t);
		vec3 outward_normal = (rec.p - center) / radius;
		rec.set_face_normal(r, outward_normal);
		rec.mat = mat;

		return true;
	}

  private:
	point3 center;
	double radius;
	shared_ptr<material> mat;
};

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

private:
	point3 center;
	vec3 normal;
	shared_ptr<material> mat;
};

#endif