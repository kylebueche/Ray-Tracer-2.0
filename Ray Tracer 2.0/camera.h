#ifndef CAMERA_H
#define CAMERA_H

#include "hittable.h"
#include "hittable_list.h"
#include "material.h"
#include <omp.h>
#include <vector>

class camera
{
  public:
	int image_width = 100;
	int image_height = 100;

	int samples_per_pixel = 10;
	int max_depth = 10; // recursive ray depth (max bounces)

	double vfov = 90; // Vertical view angle (field of view)
	point3 lookfrom = point3(0, 0, 0);
	point3 lookat = point3(0, 0, -1);
	vec3 vup = vec3(0, 1, 0);

	double defocus_angle = 0; // Variation angle of rays through each pixel
	double focus_dist = 10;   // Distance from camera lookfrom point to plane of perfect focus

	// Render the image
	void render(const hittable& scene)
	{
		initialize();

		// Dynamically send each pixel row to the next free thread, utilizes all threads by default
		std::clog << "Computing...\n";
		#pragma omp parallel for shared(scene) schedule(dynamic)
		for (int j = 0; j < image_height; j++)
		{
			// Print how many horizontal lines are left to render
			if (omp_get_thread_num() == 0)
				std::clog << "\rPercent complete: " << (int) (100.0 * j / image_height) << "%" << std::flush;

			// Iterate through each pixel in the row
			for (int i = 0; i < image_width; i++)
				shade_pixel(i, j, scene);
		}

		// Write colors out to the image file
		std::clog << "\nWriting file...\n";
		write_header(image_width, image_height);
		for (int j = 0; j < image_height; j++)
		{
			std::clog << "\rPercent complete: " << (int) (100.0 * j / image_height) << "%" << std::flush;
			for (int i = 0; i < image_width; i++)
			{
				write_color(std::cout, color_buffer[i][j]);
			}
		}

		std::clog << "\rDone.                 \n";
	}

private:
	double pixel_samples_scale;	// Color scale factor for a sum of pixel samples
	point3 center;				// Camera center
	point3 pixel00_loc;			// Location of pixel 0, 0
	vec3 pixel_delta_u;			// Offset to pixel to the right
	vec3 pixel_delta_v;			// Offset to pixel below
	vec3 u, v, w;				// Camera frame basis vectors
	vec3 defocus_disk_u;		// Defocus disk horizontal radius
	vec3 defocus_disk_v;		// Defocus disk vertical radius
	std::vector<std::vector<color>> color_buffer; // Color buffer for parallelization

	void initialize()
	{
		/* Ensure height and width are >= 1, and initialize color buffer with dimensions */
		image_width = (image_width < 1) ? 1 : image_width;
		image_height = (image_height < 1) ? 1 : image_height;
		color_buffer = std::vector<std::vector<color>>(image_width, std::vector<color>(image_height, color(0, 0, 0)));

		/* Predivide ratio for averaging, because iterated division is slow */
		pixel_samples_scale = 1.0 / samples_per_pixel;
		center = lookfrom;

		/* Determine viewport dimensions */
		auto theta = degrees_to_radians(vfov);
		auto h = std::tan(theta / 2);
		auto viewport_height = 2 * h * focus_dist;
		auto viewport_width = viewport_height * (double(image_width) / image_height);

		// Calculate the u, v, w unit basis vectors for the camera coordinate frame.
		w = unit_vector(lookfrom - lookat);
		u = unit_vector(cross(vup, w));
		v = cross(w, u);

		// Calculate the vectors across the horizontal and vertical delta vectors from pixel to pixel.
		vec3 viewport_u = viewport_width * u;   // Vector across horizontal edge
		vec3 viewport_v = viewport_height * -v; // Vector down vertical edge

		// Calculate the horizontal and vertical delta vectors from pixel to pixel.
		pixel_delta_u = viewport_u / image_width;
		pixel_delta_v = viewport_v / image_height;

		// Calculate location of upper left pixel
		auto viewport_upper_left = center - focus_dist * w - viewport_u / 2 - viewport_v / 2;
		pixel00_loc = viewport_upper_left + 0.5 * (pixel_delta_u + pixel_delta_v);

		// Calculate the camera defocus disk basis vectors
		auto defocus_radius = focus_dist * std::tan(degrees_to_radians(defocus_angle / 2));
		defocus_disk_u = u * defocus_radius;
		defocus_disk_v = v * defocus_radius;
	}


	// Construct a camera ray originatinf from the defocus disk and directed at
	// a randomly sampled point around the pixel location i, j
	ray get_ray(int i, int j) const
	{
		auto offset = sample_square();
		auto pixel_sample = pixel00_loc
			+ ((i + offset.x()) * pixel_delta_u)
			+ ((j + offset.y()) * pixel_delta_v);

		auto ray_origin = (defocus_angle <= 0) ? center : defocus_disk_sample();
		auto ray_direction = pixel_sample - ray_origin;

		return ray(ray_origin, ray_direction);
	}

	void shade_pixel(int i, int j, const hittable& scene)
	{
		color_buffer[i][j] = color(0, 0, 0);
		for (int sample = 0; sample < samples_per_pixel; sample++)
		{
			ray r = get_ray(i, j);
			color_buffer[i][j] += ray_color(r, max_depth, scene);
		}
		color_buffer[i][j] = pixel_samples_scale * color_buffer[i][j];
	}

	// Returns the vector to a random point in the [-.5,-.5],[+.5,+.5] unit square
	vec3 sample_square() const
	{
		return vec3(random_double() - 0.5, random_double() - 0.5, 0);
	}

	// Returns a random point in the camera defocus disk
	vec3 defocus_disk_sample() const
	{
		auto p = random_in_unit_disk();
		return center + (p[0] * defocus_disk_u) + (p[1] * defocus_disk_v);
	}

	color ray_color(const ray& r, int depth, const hittable& world)
	{
		// If we've exceeded the ray bounce limit, no more light is gathered
		if (depth <= 0)
			return color(0, 0, 0);

		hit_record rec;

		if (world.hit(r, interval(0.001, infinity), rec))
		{
			ray scattered;
			color attenuation;
			if (rec.mat->scatter(r, rec, attenuation, scattered))
			{
				// Dont bother recursion if the values will be 0 anyways
				//if (attenuation.near_zero())
					//return attenuation;
				// Otherwise, recursion time >:)
				return attenuation * ray_color(scattered, depth - 1, world);
			}
			return color(0, 0, 0);
		}

		vec3 unit_direction = unit_vector(r.direction());
		auto a = 0.5 * (unit_direction.y() + 1.0);
		return (1.0 - a) * color(1.0, 1.0, 1.0) + a * color(0.5, 0.7, 1.0);
	}
};

class standard_camera : public camera
{
public:

	double aspect_ratio = 1.0;
	bool auto_height = false; // Whether or not to use aspect ratio to determine height

	void render(const hittable& scene)
	{
		if (auto_height)
			image_height = int(image_width / aspect_ratio);
		camera::render(scene);
	}

	void set_dimensions(int width, int height)
	{
		auto_height = false;
		image_width = width;
		image_height = height;
	}

	/* 240p, 320x240, 4.0/3.0 */
	void setLD() { set_dimensions(320, 240); }

	/* 480p, 640x480, 4.0/3.0 */
	void setSD() { set_dimensions(640, 480); }

	/* 720p, 1280x720, 16.0/9.0 */
	void setHD() { set_dimensions(1280, 720); }

	/* 1080p, 1920x1080, 16.0/9.0 */
	void setFHD() { set_dimensions(1920, 1080); }

	/* 1440p, 2560x1440, 16.0/9.0 */
	void setQHD() { set_dimensions(2560, 1440); }

	/* 4K, 3840x2160, 16.0/9.0 */
	void setUHD() { set_dimensions(3840, 2160); }

	/* DCI 4K, 4096x2160 (Cinema Standard), 256.0/135.0 */
	void setDCI4K() { set_dimensions(4096, 2160); }

};

#endif