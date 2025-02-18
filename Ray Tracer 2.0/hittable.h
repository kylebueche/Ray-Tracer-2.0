#ifndef HITTABLE_H
#define HITTABLE_H

class material;

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

class hittable {
  public:
	virtual ~hittable() = default;

	/* Whether or not the ray hits, records the first hit in the ray's bounds */
	virtual bool hit(const ray& r, interval ray_t, hit_record& rec) const = 0;

	/* Whether or not the volume contains the point, useful for boolean geometry operations */
	virtual bool volume_contains(const point3 p) const = 0;
};

/* Solves the quadratic equation for t given an a, b, and c, returns the first hit in the ray's bounds*/
bool solve_quadratic(const ray& ray, interval ray_bounds, hit_record& record, double a, double b, double c)
{
	auto h = -2.0 * b; // Simplifies the quadratic equation with substitution
	auto inside_sqrt = h * h - a * c;

	// No intersection OR infinite intersection
	if (inside_sqrt < 0.0 || a == 0.0)
	{
		return false;
	}

	auto sqrtd = std::sqrt(inside_sqrt);

	// Ensure t0 is the closer intersection. If the denominator is negative, this will happen
	double t0 = (-b - sqrtd) / a;
	double t1 = (-b + sqrtd) / a;
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

#endif