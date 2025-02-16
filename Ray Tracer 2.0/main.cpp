/************************************************************************
* Author: The Nacho Guy idk
* 
* This code is intended to output high detail ray-traced image files in
* a reasonably optimal amount of time, but will not attempt to ray-trace
* in real time.
* 
*************************************************************************/

#include "rtproject.h";

#include "camera.h"
#include "hittable.h";
#include "hittable_list.h"
#include "material.h"
#include "sphere.h"

#include <windows.h>


int main()
{
	hittable_list scene;

	auto ground_mat = make_shared<lambertian>(color(0.5, 0.5, 0.5));
	scene.add(make_shared<plane>(point3(0, 0, 0), vec3(0, 1, 0), ground_mat));

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

	camera cam;

	cam.aspect_ratio = 16.0 / 9.0;
	cam.image_width = 800;
	cam.samples_per_pixel = 16;
	cam.max_depth = 4;

	cam.vfov = 20;
	cam.lookfrom = point3(13, 2, 3);
	cam.lookat = point3(0, 0, 0);
	cam.vup = vec3(0, 1, 0);

	cam.defocus_angle = 0.6;
	cam.focus_dist = 10.0;

	cam.render(scene);
}

void begin_csv(void);
void write_to_csv(vec3, vec3, vec3, vec3);
void csv_ray_distribution(int);

void csv_ray_distribution(int samps)
{
	vec3 norm = vec3(0, 1, 0);

	begin_csv();

	for (int i = 0; i < samps; i++)
	{
		write_to_csv(random_on_hemisphere(norm) * vec3(1, 2, 1),
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
	std::cout << onhemi.e[0] << "," << onhemi.e[1] << "," << onhemi.e[2] << "," << dot(normal, unit_vector(onhemi)) << ", ,";
	std::cout << onsphere.e[0] << "," << onsphere.e[1] << "," << onsphere.e[2] << "," << dot(normal, unit_vector(onsphere)) << ", ,";
	std::cout << insphere.e[0] << "," << insphere.e[1] << "," << insphere.e[2] << "," << dot(normal, unit_vector(insphere)) << "\n";
}
