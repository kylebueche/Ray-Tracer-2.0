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

int main()
{
	hittable_list scene;

	auto ground_mat = make_shared<lambertian>(color(0.8, 0.8, 0.0));
	auto m_sphere_mat = make_shared<lambertian>(color(0.1, 0.2, 0.5));
	auto l_sphere_mat = make_shared<metal>(color(0.8, 0.8, 0.8), 0.3);
	auto r_sphere_mat = make_shared<metal>(color(0.8, 0.6, 0.2), 1.0);

	scene.add(make_shared<sphere>(point3(0.0, -100.5, -1.0), 100.0, ground_mat));
	scene.add(make_shared<sphere>(point3(0.0,    0.0, -1.2),   0.5, m_sphere_mat));
	scene.add(make_shared<sphere>(point3(-1.0,   0.0, -1.0),   0.5, l_sphere_mat));
	scene.add(make_shared<sphere>(point3( 1.0,   0.0, -1.0),   0.5, r_sphere_mat));

	camera cam;

	cam.aspect_ratio = 16.0 / 9.0;
	cam.image_width = 400;
	cam.samples_per_pixel = 100;
	cam.max_depth = 50;

	cam.render(scene);
}
