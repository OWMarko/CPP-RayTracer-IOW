#ifndef HITTABLE_H
#define HITTABLE_H

#include "ray.hpp"
#include "rtweekend.hpp" // shared_ptr and utility functions

class material; // to avoid dependency with material.hpp

// Structure to store all relevant data about a ray-object intersection
// p : The exact intersection point in 3D space
// normal : The vector perpendicular to the surface at point p
// t : The distance along the ray 
// mat_ptr: Pointer to the material properties of the object hit
// front_face: Boolean to track if the ray hit the object from the outside or inside

struct hit_record {
    point3 p;
    vec3 normal;
    shared_ptr<material> mat_ptr;
    double t;
    bool front_face;

    // If the ray hits from outside the normal points outward
    // If the ray hits from inside (with a glass material) the normal points inward
    inline void set_face_normal(const ray& r, const vec3& outward_normal)
    {
        front_face = dot(r.direction(), outward_normal) < 0;
        normal = front_face ? outward_normal : -outward_normal;
    }
};

// Abstract Base Class for any object that can be hit by a ray
// Every bubble, square, box and so on must implement the hit function

class hittable {
    public:
        // For abstract classes to ensure proper cleanup
        virtual ~hittable() = default;

        // Ray r hit you between t_min and t_max ?
        // If yes we fill the rec structure with details and return true
        virtual bool hit(const ray& r, double t_min, double t_max, hit_record& rec) const = 0;
};

#endif