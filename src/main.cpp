#include "rtweekend.hpp"

#include "camera.hpp"
#include "hittable_list.hpp"
#include "material.hpp"
#include "sphere.hpp"

#include <iostream>
#include <chrono> 

// Recursive function to calculate the color of a ray
// First of all we have to check if the bounce limit has been exceeded, if so black is returned (there isn't light)
// Then we look intersection with the world (set at 0.001 to avoid "Shadow Acne", we can create a variable double eps also)
// Finally if the material scatters the ray we continue recursively until it disperses

color ray_color(const ray& r, const hittable& world, int depth) {
    hit_record rec;

    if (depth <= 0)
    {
        return color(0,0,0);
    }

    if (world.hit(r, 0.001, infinity, rec))
    {
        ray scattered;
        color attenuation;

        // Check if material scatters the light

        if (rec.mat_ptr->scatter(r, rec, attenuation, scattered))
        {
            return attenuation * ray_color(scattered, world, depth-1);
        }

        // If it hits but doesn't scatter (absorbed) it return black

        return color(0,0,0);
    }

    // We set a blue sky but you can set other colors if you want
    
    vec3 unit_direction = unit_vector(r.direction());
    auto t = 0.5*(unit_direction.y() + 1.0);
    return (1.0-t)*color(1.0, 1.0, 1.0) + t*color(0.5, 0.7, 1.0);
}

// There is a function to generate the random scene used for the final render (like the Book Cover of the tutorial)
// We create the ground which is represented by a giant sphere
// Then we generate a grid of small random spheres choosing their material (different materials like Diffuse, Metal, Glass) based on probabilities
// For the esthetic and good code conduct we ensure these small spheres don't intersect with the fixed large ones (we can't handle it for now)
// Finally we add the three main distinctive spheres and return the hittable list

hittable_list random_scene() {
    hittable_list world;

    auto ground_material = make_shared<lambertian>(color(0.5, 0.5, 0.5));
    world.add(make_shared<sphere>(point3(0,-1000,0), 1000, ground_material));

    for(int a = -11; a < 11; a++) 
    {
        for(int b = -11; b < 11; b++) 
        {
            auto choose_mat = random_double();
            point3 center(a + 0.9*random_double(), 0.2, b + 0.9*random_double());

            // We check distance to avoid overlapping with the big sphere at (4, 0.2, 0)
            if ((center - point3(4, 0.2, 0)).length() > 0.9) 
            {
                shared_ptr<material> sphere_material;

                if (choose_mat < 0.8)
                {
                    // Diffuse : albedo * albedo minimizes the probability of light colors (it's gamma approximation)
                    // Reminder : albedo is portion of solar radiation that is reflected back into the atmosphere
                    auto albedo = color::random() * color::random();
                    sphere_material = make_shared<lambertian>(albedo);
                    world.add(make_shared<sphere>(center, 0.2, sphere_material));

                } 
                
                else if (choose_mat < 0.95)
                {
                    // Metal
                    auto albedo = color::random(0.5, 1);
                    auto fuzz = random_double(0, 0.5);
                    sphere_material = make_shared<metal>(albedo, fuzz);
                    world.add(make_shared<sphere>(center, 0.2, sphere_material));
                } 
                
                else
                {
                    // Glass
                    sphere_material = make_shared<dielectric>(1.5);
                    world.add(make_shared<sphere>(center, 0.2, sphere_material));
                }
            }
        }
    }

    // The 3 main large spheres
    auto material1 = make_shared<dielectric>(1.5);
    world.add(make_shared<sphere>(point3(0, 1, 0), 1.0, material1));

    auto material2 = make_shared<lambertian>(color(0.4, 0.2, 0.1));
    world.add(make_shared<sphere>(point3(-4, 1, 0), 1.0, material2));

    auto material3 = make_shared<metal>(color(0.7, 0.6, 0.5), 0.0);
    world.add(make_shared<sphere>(point3(4, 1, 0), 1.0, material3));

    return world;
}

int main() {
    // We define image Settings, first the resolution and quality parameters here
    // We use a small width (400) for quick debugging/testing
    // But for the final render we increase to 1200+ and samples_per_pixel to 100+
    // We don't forget to initialise max_depth limits the recursion stack to prevent infinite bounces

    const auto aspect_ratio = 16.0 / 9.0;
    const int image_width = 400; 
    const int image_height = static_cast<int>(image_width / aspect_ratio);
    const int samples_per_pixel = 10;
    const int max_depth = 50;

    // Then we're taking care of world and camera setup 
    // We generate the random scene and setup the camera positioning
    // Aperture controls the size of the lens (Defocus Blur / Depth of Field)
    // dist_to_focus determines the plane of perfect focus

    auto world = random_scene();
    point3 lookfrom(13,2,3);
    point3 lookat(0,0,0);
    vec3 vup(0,1,0);
    auto dist_to_focus = 10.0;
    auto aperture = 0.1;

    camera cam(lookfrom, lookat, vup, 20, aspect_ratio, aperture, dist_to_focus);

    // Without forgetting the render loop
    // First we write the PPM Header (P3 format)
    // Then we iterate over each pixel, using a high_resolution_clock to benchmark performance
    // For each pixel, we perform multi-sampling (MSAA) to reduce aliasing and noise
    // Finally we apply gamma correction and write the color values to stdout

    std::cout << "P3\n" << image_width << ' ' << image_height << "\n255\n";

    auto start = std::chrono::high_resolution_clock::now();

    for (int j = image_height-1; j >= 0; --j)
    {
        std::cerr << "\rScanlines remaining: " << j << ' ' << std::flush;
        
        // This is where we will add OpenMP parallelism with pragma omp parallel for
        for (int i = 0; i < image_width; ++i)
        {
            color pixel_color(0, 0, 0);

            // A Monte Carlo accumulation for antialiasing (We saw it also in MCMC Lectures in my master)
            for (int s = 0; s < samples_per_pixel; ++s)
            {
                auto u = (i + random_double()) / (image_width-1);
                auto v = (j + random_double()) / (image_height-1);
                ray r = cam.get_ray(u, v);
                pixel_color += ray_color(r, world, max_depth);
            }

            // We apply a color processing here, we normalize samples and apply Gamma Correction (gamma=2.0)
            auto r = pixel_color.x();
            auto g = pixel_color.y();
            auto b = pixel_color.z();

            auto scale = 1.0 / samples_per_pixel;
            r = sqrt(scale * r);
            g = sqrt(scale * g);
            b = sqrt(scale * b);

            std::cout << static_cast<int>(256 * clamp(r, 0.0, 0.999)) << ' '
                      << static_cast<int>(256 * clamp(g, 0.0, 0.999)) << ' '
                      << static_cast<int>(256 * clamp(b, 0.0, 0.999)) << '\n';
        }
    }

    // The chrono stop
    auto stop = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(stop - start);

    std::cerr << "\nDone in " << duration.count() << "ms.\n";
}