/************************************************************************
* Author: The Nacho Guy idk
* 
* This code is intended to output high detail ray-traced image files in
* a reasonably optimal amount of time, but will not attempt to ray-trace
* in real time.
* 
*************************************************************************/

#include <iostream>

int main()
{
	// The size of the image
	int image_height = 256;
	int image_width = 256;

	// Rendering the image
	std::cout << "P3\n" << image_width << ' ' << image_height << "\n255\n";

	for (int j = 0; j < image_height; j++)
	{
		std::clog << "\rScanlines remaining: " << (image_height - j) << ' ' << std::flush;
		for (int i = 0; i < image_width; i++)
		{
			auto r = double(i) / (image_width - 1);
			auto g = double(j) / (image_height - 1);
			auto b = 0.0;

			int ir = int(255.999 * r);
			int ig = int(255.99 * g);
			int ib = int(255.999 * b);

			std::cout << ir << ' ' << ig << ' ' << ib << '\n';
		}
	}
}