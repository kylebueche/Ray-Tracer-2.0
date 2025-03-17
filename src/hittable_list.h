#ifndef HITTABLE_LIST_H
#define HITTABLE_LIST_H

#include "hittable.h"

#include <vector>

class hittable_list : public hittable
{
  public:
	std::vector<shared_ptr<hittable>> objects;
	hittable_list() {}
	hittable_list(shared_ptr<hittable> object) { add(object); }

	void clear() { objects.clear(); }

	void add(shared_ptr<hittable> object)
	{
		objects.push_back(object);
	}

	bool hit(const ray& r, interval ray_t , hit_record& rec) const override
	{
		hit_record temp_rec;
		bool hit_anything = false;
		auto closest_so_far = ray_t.max;

		for (const auto& object : objects)
		{
			if (object->hit(r, interval(ray_t.min, closest_so_far), temp_rec))
			{
				hit_anything = true;
				closest_so_far = temp_rec.t;
				rec = temp_rec;
			}
		}

		return hit_anything;
	}

	virtual bool volume_contains(const point3 p) const override
	{
		return true;
	}
};


class hittable_intersection : public hittable_list
{
public:

	bool hit(const ray& ray, interval ray_bounds, hit_record& record) const override
	{
		hit_record temp_rec;
		bool hit_anything = false;
		auto closest_so_far = ray_bounds.max;

		for (const auto& object : objects)
		{
			if (object->hit(ray, interval(ray_bounds.min, closest_so_far), temp_rec))
			{
				bool intersecting = true;
				for (const auto& object2 : objects)
				{
					if (object != object2 && !object2->volume_contains(temp_rec.p))
						intersecting = false;
				}
				if (intersecting)
				{
					hit_anything = true;
					closest_so_far = temp_rec.t;
					record = temp_rec;
				}
			}
		}

		return hit_anything;
	}


	virtual bool volume_contains(const point3 p) const override
	{
		bool all_contain = true;
		for (const auto& object : objects)
		{
			if (!object->volume_contains(p))
				all_contain = false;
		}
		return all_contain;
	}
};

#endif