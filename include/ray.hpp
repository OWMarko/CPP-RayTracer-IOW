#ifndef RAY_H
#define RAY_H

#include "vec3.hpp"

// This class representing a ray in 3D space
// Mathematically, a ray is a function P(t) = A + t*b where :
// A is the ray origin
// b is the ray direction
// t is a scalar parameter. By changing this parameter we move the point P(t) along the ray
// Naturally a positive scalar t means in front of the origin and negative t is behind

class ray {
    public:
        point3 orig;
        vec3 dir;

    public:
        // Constructors
        ray() {}
        ray(const point3& origin, const vec3& direction)
            : orig(origin), dir(direction)
        {}

        // Accessors
        point3 origin() const  { return orig; }
        vec3 direction() const { return dir; }

        // Returns the point at parameter t
        // P(t) = origin + t * direction
        point3 at(double t) const
        {
            return orig + t*dir;
        }
};

#endif