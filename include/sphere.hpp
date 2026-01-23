#ifndef SPHERE_H
#define SPHERE_H

#include "hittable.hpp"
#include "vec3.hpp"

// There is a class representing a 3D Sphere defined by a center point and a radius
// It inherits from "hittable" because it must implement the "hit" function used by the ray tracer
// Ideally we store a pointer to a material so the sphere knows how it interacts with light

class sphere : public hittable {
    public:
        point3 center;
        double radius;
        shared_ptr<material> mat_ptr;

    public:
        // Constructors
        sphere() {}
        sphere(point3 cen, double r, shared_ptr<material> m)
            : center(cen), radius(r), mat_ptr(m) {};

        // We substitute the ray equation P(t) = A + tb into the sphere equation (P-C).(P-C) = r^2
        // This gives us a quadratic equation: t^2(b.b) + 2t(b.(A-C)) + ((A-C).(A-C) - r^2) = 0
        // We solve for t to find intersection points
        
        virtual bool hit(const ray& r, double t_min, double t_max, hit_record& rec) const override
        {
            vec3 oc = r.origin() - center;
            
            // Calculating coefficients for the quadratic formula ax^2 + bx + c = 0
            // We use half_b to remove the factor of 2 by doing this we simplify the formula 

            auto a = r.direction().length_squared();
            auto half_b = dot(oc, r.direction());
            auto c = oc.length_squared() - radius*radius;

            auto discriminant = half_b*half_b - a*c;
            if (discriminant < 0)
            {
                return false;
            }
            auto sqrtd = sqrt(discriminant);

            // Find the nearest root that lies in [t_min, t_max]

            auto root = (-half_b - sqrtd) / a;
            if (root < t_min || root > t_max)
            {
                root = (-half_b + sqrtd) / a;
                if (root < t_min || root > t_max)
                {
                    return false;
                }
                
            }

            // If we are here the ray hit the sphere

            rec.t = root;
            rec.p = r.at(rec.t);
            
            // Calculate the vector perpendicular to surface pointing out i.e. the normal outward
            // (Point - Center) / Radius gives a unit vector

            vec3 outward_normal = (rec.p - center) / radius;
            
            // Determine if the ray hit the outside or inside face

            rec.set_face_normal(r, outward_normal);
            rec.mat_ptr = mat_ptr;

            return true;
        }
};

#endif