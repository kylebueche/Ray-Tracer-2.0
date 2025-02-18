#ifndef INFINITE_CONE_H
#define INFINITE_CONE_H

#include "hittable.h"

class infinite_cone : public hittable
{
public:
	infinite_cone(const point3& center, const vec3& axis, double angle, shared_ptr<material> mat)
		: center(center), axis(axis), angle(angle), mat(mat) {}

	bool hit(const ray& ray, interval ray_bounds, hit_record& record) const override
	{
		// Normalize axis so it's length doesn't have to be accounted for
		vec3 normal_axis = unit_vector(axis);
		vec3 center_to_rayorig = ray.origin() - center;

		// Reused cosines
		double cos_angle = cos(degrees_to_radians(angle));
		double cos_sqr = cos_angle * cos_angle;

		// Reused values
		double axis_dot_raydir = dot(normal_axis, ray.direction());
		double raydir_lensq = ray.direction().length_squared();
		double axis_dot_center_to_rayorig = dot(axis, center_to_rayorig);

		double a = axis_dot_raydir * axis_dot_raydir - (cos_sqr * raydir_lensq);
		double b = 2 * ((axis_dot_raydir * axis_dot_center_to_rayorig) - cos_sqr * dot(center_to_rayorig, ray.direction()));
		double c = axis_dot_center_to_rayorig * axis_dot_center_to_rayorig - cos_sqr * (ray.origin().length_squared() - 2 * dot(ray.origin(), center) + center.length_squared());

		solve_quadratic(ray, ray_bounds, record, a, b, c);
		if (dot(record.p - center, axis) < 0.0)
			return false;
		record.set_face_normal(ray, unit_vector(cross(record.p, cross(record.p, normal_axis))));
		record.mat = mat;
		//record.hit(ray, t0, mat, normal);
		return true;
	}

	/* Implicit volume in the direction of the axis, in a given zenith around the axis */
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