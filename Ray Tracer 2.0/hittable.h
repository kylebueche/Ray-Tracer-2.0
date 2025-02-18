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


	void hit(const ray& r, const double t_hit, const shared_ptr<material>& mat_hit, const vec3& outward_normal)
	{
		t = t_hit;
		p = r.at(t_hit);
		mat = mat_hit;
		set_face_normal(r, outward_normal);
	}

	// Set the hit record's normal vector.
	// Assumes normalized vectors.
	void set_face_normal(const ray& r, const vec3& outward_normal)
	{
		front_face = dot(r.direction(), outward_normal) < 0;
		normal = front_face ? outward_normal : -outward_normal;
	}
};

class hittable {
  public:
	virtual ~hittable() = default;

	virtual bool hit(const ray& r, interval ray_t, hit_record& rec) const = 0;

	virtual bool volume_contains(const point3 p) const = 0;
};

#endif