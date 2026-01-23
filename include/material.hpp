#ifndef MATERIAL_H
#define MATERIAL_H

#include "rtweekend.hpp"
#include "hittable.hpp" // Required because materials need to know hit_record

struct hit_record;

// Abstract Base Class for Materials
// scatter: decides how an incoming ray reflects off a surface
// If the ray is absorbed scatter returns false
// If it reflects/refracts it returns true and populates the scattered ray and attenuation color

class material {
    public:
        virtual bool scatter(
            const ray& r_in, const hit_record& rec, color& attenuation, ray& scattered
        ) const = 0;
};

// Lambertian Material simulates matte surfaces like chalk and paper
// Light that hits the surface is scattered in a random direction but with a distribution
// closer to the normal (Lambert's Cosine Law approximation)

class lambertian : public material {
    public:
        color albedo; // The base color of the material

        lambertian(const color& a) : albedo(a) {}

        virtual bool scatter(
            const ray& r_in, const hit_record& rec, color& attenuation, ray& scattered
        ) const override
        {
            // We pick a random point on the unit sphere tangent to the hit point
            // This creates the diffuse scattering effect

            auto scatter_direction = rec.normal + random_unit_vector();

            // Catch degenerate scatter direction
            // If the random vector is exactly opposite to the normal the sum equals zero
            // leading to NaNs/Infinite recursion later. We handle this by resetting to normal

            if (scatter_direction.near_zero())
                scatter_direction = rec.normal;

            scattered = ray(rec.p, scatter_direction);
            attenuation = albedo;
            return true;
        }
};

// Metal Material simulates shiny surfaces like mirror, it's a reflective material
// It uses perfect reflection logic e.g. Angle of Incidence = Angle of Reflection
// Fuzz parameter allows simulating brushed metals or imperfect mirrors by slightly randomizing the reflected ray direction.

class metal : public material {
    public:
        color albedo;
        double fuzz;

        metal(const color& a, double f) : albedo(a), fuzz(f < 1 ? f : 1) {}

        virtual bool scatter(
            const ray& r_in, const hit_record& rec, color& attenuation, ray& scattered
        ) const override
        {
            // Calculate perfect reflection vector

            vec3 reflected = reflect(unit_vector(r_in.direction()), rec.normal);
            
            // Add randomness fuzz inside a small sphere at the tip of the reflected ray

            scattered = ray(rec.p, reflected + fuzz*random_in_unit_sphere());
            attenuation = albedo;
            
            // Only scatter if the ray is not going into the object (reflecting outward)

            return (dot(scattered.direction(), rec.normal) > 0);
        }
};

// Dielectric Material handles Refraction (light bending) and Reflection like water
// It uses Snell's Law to calculate the bending angle based on the refractive index ir
// It also implements Schlick's Approximation to handle the fact that glass becomes 
// mirror-like at a total internal reflection

class dielectric : public material {
    public:
        double ir; // Index of Refraction (1.5 for glass, 1.33 for water, 2.4 for diamond)

        dielectric(double index_of_refraction) : ir(index_of_refraction) {}

        virtual bool scatter(
            const ray& r_in, const hit_record& rec, color& attenuation, ray& scattered
        ) const override
        {
            attenuation = color(1.0, 1.0, 1.0); // Glass absorbs little light (white)
            double refraction_ratio = rec.front_face ? (1.0/ir) : ir;

            vec3 unit_direction = unit_vector(r_in.direction());
            double cos_theta = fmin(dot(-unit_direction, rec.normal), 1.0);
            double sin_theta = sqrt(1.0 - cos_theta*cos_theta);

            bool cannot_refract = refraction_ratio * sin_theta > 1.0;
            vec3 direction;

            // If we cannot refract, there is a total internal reflection, OR if Schlick's approximation 
            // says the reflection probability is high (grazing angle) we reflect

            if (cannot_refract || reflectance(cos_theta, refraction_ratio) > random_double())
            {
                direction = reflect(unit_direction, rec.normal);
            }
            else
            {
                direction = refract(unit_direction, rec.normal, refraction_ratio);
            }

            scattered = ray(rec.p, direction);
            return true;
        }

    private:
        // Schlick's approximation for reflectance
        static double reflectance(double cosine, double ref_idx)
        {
            auto r0 = (1-ref_idx) / (1+ref_idx);
            r0 = r0*r0;
            return r0 + (1-r0)*pow((1 - cosine), 5);
        }
};

#endif