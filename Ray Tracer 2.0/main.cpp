/************************************************************************
* Author: The Nacho Guy
* 
* This code is intended to output high detail ray-traced image files in
* a reasonably optimal amount of time, but will not attempt to ray-trace
* in real time.
* 
*************************************************************************/

#include "rtproject.h"

#include "camera.h"
#include "hittable.h"
#include "hittable_list.h"
#include "material.h"
#include "sphere.h"
#include "plane.h"
#include "infinite_cone.h"

#include <omp.h>
#include <windows.h>

void intersection_geometry_scene(void);
void cone_scene(void);
void csv_ray_distribution(int);

int main()
{
	cone_scene();
}

void cone_scene()
{

	hittable_list scene;

	auto ground_mat = make_shared<lambertian>(color(0.5, 0.8, 0.5));
	scene.add(make_shared<plane>(point3(0, 0, 0), vec3(0, 1, 0), ground_mat));
	for (int i = 0; i < 3; i++)
	{
		double z = i * 2.1 - 2.1;
		double angle = i * 20 + 20;
		shared_ptr<hittable_intersection> inter = make_shared<hittable_intersection>();
		auto cone_mat_shiny = make_shared<lambertian>(color(0.9, 0.1, 0.1));
		inter->add(make_shared<infinite_cone>(point3(0.0, 0.0, z), vec3(0, 1, 0), angle, cone_mat_shiny));
		inter->add(make_shared<infinite_cone>(point3(0.0, 0.01, z), vec3(0, -1, 0), 180 - angle, cone_mat_shiny));
		inter->add(make_shared<sphere>(point3(0.0, 1.0, z), 1.0, cone_mat_shiny));
		scene.add(inter);

		auto mat1 = make_shared<dielectric>(1.0001);
		scene.add(make_shared<sphere>(point3(0, 1, z), 1.001, mat1));
	}
	standard_camera cam;

	cam.setSD();

	cam.samples_per_pixel = 100;
	cam.max_depth = 15;

	cam.vfov = 35;
	cam.lookfrom = point3(7.75, 2.25, 0);
	cam.lookat = point3(0, 1, 0);
	cam.vup = vec3(0, 1, 0);

	cam.defocus_angle = 0.0;
	cam.focus_dist = (cam.lookat - cam.lookfrom).length();
	cam.render(scene);
}

void intersection_geometry_scene()
{
	hittable_list scene;

	auto hill = make_shared<hittable_intersection>();
	auto ground_mat = make_shared<lambertian>(color(0.5, 0.5, 0.5));
	hill->add(make_shared<plane>(point3(0, 0, 3), vec3(0, 1, 0), ground_mat));
	hill->add(make_shared<plane>(point3(0, 0, 12), vec3(0, 0, 1), ground_mat));
	hill->add(make_shared<plane>(point3(12, 0, 0), vec3(1, 0, 0), ground_mat));
	hill->add(make_shared<plane>(point3(0, 0, -12), vec3(0, 0, -1), ground_mat));
	hill->add(make_shared<plane>(point3(-12, 0, 0), vec3(-1, 0, 0), ground_mat));
	scene.add(hill);

	scene.add(make_shared<plane>(point3(0, -1, 0), vec3(0, 1, 0), ground_mat));

	shared_ptr<hittable_intersection> intersection = make_shared<hittable_intersection>();
	auto cone_mat = make_shared<lambertian>(color(1, 0.0, 0.0));
	auto cone_mat_shiny = make_shared<metal>(color(0.9, 0.1, 0.1), 0.01);
	intersection->add(make_shared<infinite_cone>(point3(4, 0.0, 8.0), vec3(0, 1, 0), 30, cone_mat));
	intersection->add(make_shared<sphere>(point3(4, 1.0, 8), 1.0, cone_mat));
	intersection->add(make_shared<plane>(point3(4, 1.4, 8.6), vec3(0.3, 0.2, 1), cone_mat));
	scene.add(intersection);

	shared_ptr<hittable_intersection> intersection2 = make_shared<hittable_intersection>();
	intersection2->add(make_shared<sphere>(point3(8.0, 0.5, 1.0), 1.0, cone_mat_shiny));
	intersection2->add(make_shared<sphere>(point3(8.5, 0.5, 0.2), 1.0, cone_mat_shiny));
	scene.add(intersection2);

	for (int a = -11; a < 11; a++)
	{
		for (int b = -11; b < 11; b++)
		{
			auto choose_mat = random_double();
			point3 center(a + 0.9 * random_double(), 0.2, b + 0.9 * random_double());

			if ((center - point3(4, 0.2, 0)).length() > 0.9)
			{
				shared_ptr<material> sphere_mat;
				
				if (choose_mat < 0.10)
				{
					// diffuse
					auto albedo = color::random() * color::random();
					sphere_mat = make_shared<lambertian>(albedo);
				}
				else if (choose_mat < 0.95)
				{
					// metal
					auto albedo = color::random(0.5, 1);
					auto fuzz = random_double(0, 0.5);
					sphere_mat = make_shared<metal>(albedo, fuzz);
				}
				else
				{
					// glass
					sphere_mat = make_shared<dielectric>(1.5);
				}
				scene.add(make_shared<sphere>(center, 0.2, sphere_mat));
			}

			
		}
	}

	auto mat1 = make_shared<dielectric>(1.5);
	scene.add(make_shared<sphere>(point3(0, 1, 0), 1.0, mat1));

	auto mat2 = make_shared<lambertian>(color(0.4, 0.2, 0.1));
	scene.add(make_shared<sphere>(point3(-4, 1, 0), 1.0, mat2));

	auto mat3 = make_shared<metal>(color(0.7, 0.6, 0.5), 0.0);
	scene.add(make_shared<sphere>(point3(4, 1, 0), 1.0, mat3));

	standard_camera cam;

	cam.setLD();

	cam.samples_per_pixel = 100;
	cam.max_depth = 15;

	cam.vfov = 30;
	cam.lookfrom = point3(20, 3, 20);
	cam.lookat = point3(0, 0, 0);
	cam.vup = vec3(0, 1, 0);

	cam.defocus_angle = 0.0;
	cam.focus_dist = (cam.lookat - cam.lookfrom).length();
	cam.render(scene);
}

void rt_one_weekend_final_scene() {
	hittable_list world;

	auto ground_material = make_shared<lambertian>(color(0.5, 0.5, 0.5));
	world.add(make_shared<sphere>(point3(0, -1000, 0), 1000, ground_material));

	for (int a = -11; a < 11; a++) {
		for (int b = -11; b < 11; b++) {
			auto choose_mat = random_double();
			point3 center(a + 0.9 * random_double(), 0.2, b + 0.9 * random_double());

			if ((center - point3(4, 0.2, 0)).length() > 0.9) {
				shared_ptr<material> sphere_material;

				if (choose_mat < 0.8) {
					// diffuse
					auto albedo = color::random() * color::random();
					sphere_material = make_shared<lambertian>(albedo);
					world.add(make_shared<sphere>(center, 0.2, sphere_material));
				}
				else if (choose_mat < 0.95) {
					// metal
					auto albedo = color::random(0.5, 1);
					auto fuzz = random_double(0, 0.5);
					sphere_material = make_shared<metal>(albedo, fuzz);
					world.add(make_shared<sphere>(center, 0.2, sphere_material));
				}
				else {
					// glass
					sphere_material = make_shared<dielectric>(1.5);
					world.add(make_shared<sphere>(center, 0.2, sphere_material));
				}
			}
		}
	}

	auto material1 = make_shared<dielectric>(1.5);
	world.add(make_shared<sphere>(point3(0, 1, 0), 1.0, material1));

	auto material2 = make_shared<lambertian>(color(0.4, 0.2, 0.1));
	world.add(make_shared<sphere>(point3(-4, 1, 0), 1.0, material2));

	auto material3 = make_shared<metal>(color(0.7, 0.6, 0.5), 0.0);
	world.add(make_shared<sphere>(point3(4, 1, 0), 1.0, material3));

	camera cam;

	cam.image_height = 675;
	cam.image_width = 1200;
	cam.samples_per_pixel = 500;
	cam.max_depth = 50;

	cam.vfov = 20;
	cam.lookfrom = point3(13, 2, 3);
	cam.lookat = point3(0, 0, 0);
	cam.vup = vec3(0, 1, 0);

	cam.defocus_angle = 0.6;
	cam.focus_dist = 10.0;

	cam.render(world);
}


void begin_csv(void);
void write_to_csv(vec3, vec3, vec3, vec3);

vec3 random_on_cylinder(void);
vec3 random_on_cylinder()
{

	while (true)
	{
		auto p = vec3(random_double(-1, 1), 0, random_double(-1, 1));
		if (p.length_squared() < 1)
			return (p / p.length()) + vec3(0, std::rand(), 0);
	}
}

void csv_ray_distribution(int samps)
{
	vec3 norm = vec3(0, 1, 0);

	begin_csv();

	for (int i = 0; i < samps; i++)
	{
		write_to_csv(random_on_cylinder(),//random_on_hemisphere(norm) * vec3(1, 2, 1),
			norm + random_unit_vector(),
			norm + random_in_unit_sphere(),
			norm);
	}
}

void begin_csv()
{
	std::cout << "On Hemisphere, , , , ,On Unit Sphere, , , , ,In Unit Sphere\n";
	std::cout << "X Values,Y Values,Z Values,Cosine Values, ,X Values,Y Values,Z Values,Cosine Values, ,X Values,Y Values,Z Values,Cosine Values\n";
}

void write_to_csv(vec3 onhemi, vec3 onsphere, vec3 insphere, vec3 normal)
{
	std::cout << onhemi.e[0] << "," << onhemi.e[1] << "," << onhemi.e[2] << "," << dot(normal, unit_vector(onhemi)) << "\n";
	//std::cout << onhemi.e[0] << "," << onhemi.e[1] << "," << onhemi.e[2] << "," << dot(normal, unit_vector(onhemi)) << ", ,";
	//std::cout << onsphere.e[0] << "," << onsphere.e[1] << "," << onsphere.e[2] << "," << dot(normal, unit_vector(onsphere)) << ", ,";
	//std::cout << insphere.e[0] << "," << insphere.e[1] << "," << insphere.e[2] << "," << dot(normal, unit_vector(insphere)) << "\n";
}
