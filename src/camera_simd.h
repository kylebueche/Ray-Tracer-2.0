#ifndef CAMERA_SIMD_H
#define CAMERA_SIMD_H

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "dep/stb_image_write.h"
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
        simd_render(scene);
        write_png((char *) "output/image.png");
    }

private:
    double pixel_samples_scale; // Color scale factor for a sum of pixel samples
    point3 center;              // Camera center
    point3 pixel00_loc;         // Location of pixel 0, 0
    vec3 pixel_delta_u;         // Offset to pixel to the right
    vec3 pixel_delta_v;         // Offset to pixel below
    vec3 u, v, w;               // Camera frame basis vectors
    vec3 defocus_disk_u;        // Defocus disk horizontal radius
    vec3 defocus_disk_v;        // Defocus disk vertical radius
    std::vector<color> color_buffer; // Color buffer for parallelization
    // Output SIMD parameters
    AlignedArray<float> image_r;
    AlignedArray<float> image_g;
    AlignedArray<float> image_b;

    void initialize()
    {
        image_r = AlignedArray<float>(image_height * image_width);
        image_g = AlignedArray<float>(image_height * image_width);
        image_b = AlignedArray<float>(image_height * image_width);

        /* Predivide ratio for averaging, because iterated division is slow */
        pixel_samples_scale = 1.0 / samples_per_pixel;
        center = lookfrom;

        /* Determine viewport dimensions */
        float theta = degrees_to_radians(vfov);
        float h = std::tan(theta / 2);
        float viewport_height = 2 * h * focus_dist;
        float viewport_width = viewport_height * (double(image_width) / image_height);

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


    // Construct a camera ray originating from the defocus disk and directed at
    // a randomly sampled point around the pixel location i, j

    // Shades multiple pixels in simd lockstep
    void simd_render_128f(const hittable& scene)
    {
		
		__m128 v_pixelWidth = _mm_load_ps1(pixelWidth);
		__m128 v_pixelHeight = _mm_load_ps1(pixelHeight);
		__m128 v_randState = _mm_load_ps1(;
		sse_init_rand_state_128f(v_randState);
		for (int p = 0; p < num_pixels; p = p + FLOATS_IN_128)
		{
			__m128 v_pixelX = _mm_load_ps(&pixelPosX[p]);
  			__m128 v_pixelY = _mm_load_ps(&pixelPosY[p]);
			for (int r = 0; r < num_rays; r = r++)
			{
				__m128 a;
				__m128 rayPosX, rayPosY, rayPosZ, rayDirX, rayDirY, rayDirZ;
				sse_sample_ray(v_randState, v_pixelX, v_pixelY, v_pixelWidth, v_pixelHeight, /* <- input, output -> */ rayPosX, rayPosY, rayPosZ, rayDirX, rayDirY, rayDirZ);
				sse_lensq_128f(rayDirX, rayDirY, rayDirZ, /* <- input, output -> */ a);
				for (int s = 0; s < num_spheres; o = o++)
				{
					// Advantageous to have AoS sphere objects here
					__m128 radius = _mm_load_ps1(spheres[s].radius);
					__m128 originToCenterX = _mm_sub_ps(_mm_load_ps1(spheres[s].x), rayPosX);
					__m128 originToCenterY = _mm_sub_ps(_mm_load_ps1(spheres[s].y), rayPosY);
					__m128 originToCenterZ = _mm_sub_ps(_mm_load_ps1(spheres[s].z), rayPosZ);
					// Substitute -0.5 * b for h, skip some computation
					__m128 h;
					__m128 originToCenterLenSq;
					sse_dot_128f(rayDirX, rayDirY, rayDirZ, originToCenterX, originToCenterY, originToCenterZ, /* <- input, output -> */ h);
					sse_lensq_128f(originToCenterX, originToCenterX, originToCenterX, /* <- input, output -> */ originToCenterLenSq);
					__m128 c = _mm_sub_128f(originToCenterLenSq, _mm_mul_ps(radius, radius));
					__m128 t0, t1, solution_exists;
					sse_solve_quadratic_128f(a, h, c, /* <- input, output -> */ solution_exists, t0, t1);
					__m128 t = _mm_min_ps(t0, t1);
					/* More logic here to track the closest object thus far, and do material and color stuff */
					/* Sum the colors into the pixel whatever im tired of writing prototype code */
				}
			}
		}
        
    void write_png(char *filename)
    {
        std::vector<unsigned char> out = std::vector<unsigned char>(image_height * image_width * 3, 0);
        for (int i = 0; i < image_height * image_width; i++)
        {
            double r = color_buffer[i].x();
            double g = color_buffer[i].y();
            double b = color_buffer[i].z();

            r = linear_to_gamma(r);
            g = linear_to_gamma(g);
            b = linear_to_gamma(b);

            // Translate the [0,1] component values into the byte range [0,255]
            static const interval intensity(0.000, 0.999);
            out[i * 3] = int(255.99 * intensity.clamp(r));
            out[i * 3 + 1] = int(255.99 * intensity.clamp(g));
            out[i * 3 + 2] = int(255.99 * intensity.clamp(b));
        }
        stbi_write_png(filename, image_width, image_height, 3, out.data(), sizeof(char) * 3 * image_width);
    }
};


#endif
