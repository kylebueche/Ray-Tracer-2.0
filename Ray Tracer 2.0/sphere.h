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


	virtual bool volume_contains(const point3 p) const override
	{
		return (p - center).length_squared() <= radius * radius;
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


	virtual bool volume_contains(const point3 p) const override
	{
		return dot(p - center, normal) <= 0.0;
	}

private:
	point3 center;
	vec3 normal;
	shared_ptr<material> mat;
};

class angular_plane : public hittable
{
public:
	angular_plane(const point3& center, const vec3& axis, double angle, shared_ptr<material> mat)
		: center(center), axis(axis), angle(angle), mat(mat) {}

	bool hit(const ray& ray, interval ray_bounds, hit_record& record) const override
	{
		/* Hey man, if I derived the formula, you can do it too. */
		double cos_angle = cos(angle);
		vec3 normal_axis = unit_vector(axis);
		double cos_sqr = cos_angle * cos_angle;
		double axis_dot_raydir = dot(normal_axis, ray.direction());
		double raydir_lensq = ray.direction().length_squared();
		double a = axis_dot_raydir * axis_dot_raydir - (cos_sqr * raydir_lensq);
		vec3 center_to_rayorig = ray.origin() - center;
		double axis_dot_center_to_rayorig = dot(axis, center_to_rayorig);
		double b = 2 * ((axis_dot_raydir * axis_dot_center_to_rayorig) - cos_sqr * dot(center_to_rayorig, ray.direction()));
		double c = axis_dot_center_to_rayorig * axis_dot_center_to_rayorig - cos_sqr * (ray.origin().length_squared() - 2 * dot(ray.origin(), center) + center.length_squared());

		double inside_sqrt = (b * b) - (4.0 * a * c);
		double denominator = 2.0 * a;
		if (inside_sqrt < 0.0 || denominator == 0.0)
			return false;

		double t0 = (-b - sqrt(inside_sqrt)) / denominator;
		double t1 = (-b + sqrt(inside_sqrt)) / denominator;

		// Ensure t0 is the closer intersection. If the denominator is negative, this will happen
		if (t0 > t1) std::swap(t0, t1);

		double t = t0;
		if (!ray_bounds.surrounds(t))
		{
			t = t1;
			if (!ray_bounds.surrounds(t))
			{
				return false;
			}
		}
		record.t = t;
		point3 p = ray.at(t);
		if (dot(p - center, axis) < 0.0)
			return false;
		record.t = t;
		record.p = p;
		record.set_face_normal(ray, unit_vector(cross(record.p, cross(record.p, normal_axis))));
		record.mat = mat;
		//record.hit(ray, t0, mat, normal);
		return true;
	}

	virtual bool volume_contains(const point3 p) const override
	{
		return dot(unit_vector(p - center), unit_vector(axis)) > cos(angle);
	}

private:
	point3 center;
	vec3 axis;
	double angle;
	shared_ptr<material> mat;
};


#endif