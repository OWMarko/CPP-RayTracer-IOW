#ifndef VEC3_H
#define VEC3_H

#include <cmath>
#include <iostream>

using std::sqrt;

// Declaration of utility functions found in rtweekend.hpp
// We need these for the random vector generation methods inside the class

double random_double();
double random_double(double min, double max);


// This class is the principal element of the renderer. It represents:
// 3D Geometric points (x, y, z)
// 3D Directions/Vectors (x, y, z)
// RGB Colors (r, g, b)

// Nota Bene : We can, idealy, separate Point, Vector, and Color into different types for type-safety,
// but for this tutorial, aliasing them to the same class is more convenient.

class vec3 {
    public:
        double e[3];

        // Constructors
        vec3() : e{0,0,0} {}
        vec3(double e0, double e1, double e2) : e{e0, e1, e2} {}

        // Accessors (x, y, z)
        double x() const { return e[0]; }
        double y() const { return e[1]; }
        double z() const { return e[2]; }

        // Operator overloading for vector arithmetic
        vec3 operator-() const { return vec3(-e[0], -e[1], -e[2]); }
        double operator[](int i) const { return e[i]; }
        double& operator[](int i) { return e[i]; }

        vec3& operator+=(const vec3 &v)
        {
            e[0] += v.e[0];
            e[1] += v.e[1];
            e[2] += v.e[2];
            return *this;
        }

        vec3& operator*=(const double t)
        {
            e[0] *= t;
            e[1] *= t;
            e[2] *= t;
            return *this;
        }

        vec3& operator/=(const double t)
        {
            return *this *= 1/t;
        }

        // Geometric utility methods
        double length() const
        {
            return sqrt(length_squared());
        }

        double length_squared() const
        {
            return e[0]*e[0] + e[1]*e[1] + e[2]*e[2];
        }

        // Random Number Generation methods (Used for Materials/Monte Carlo)
        // Requires random_double() to be defined/linked in rtweekend.hp

        inline static vec3 random()
        {
            return vec3(random_double(), random_double(), random_double());
        }

        inline static vec3 random(double min, double max)
        {
            return vec3(random_double(min,max), random_double(min,max), random_double(min,max));
        }

        // Returns a random vector inside the unit sphere by Rejection Method
        // Used for Lambertian diffuse reflection
        bool near_zero() const
        {
            // Return true if the vector is close to zero in all dimensions.
            const auto s = 1e-8;
            return (std::abs(e[0]) < s) && (std::abs(e[1]) < s) && (std::abs(e[2]) < s);
        }
};

// Type Aliases for code clarity

using point3 = vec3;   // 3D Point
using color = vec3;    // RGB Color

// Utility Functions (Non-member)
// These allow writing "v1 + v2" or "cout << v1"

inline std::ostream& operator<<(std::ostream &out, const vec3 &v) 
{
    return out << v.e[0] << ' ' << v.e[1] << ' ' << v.e[2];
}

inline vec3 operator+(const vec3 &u, const vec3 &v)
{
    return vec3(u.e[0] + v.e[0], u.e[1] + v.e[1], u.e[2] + v.e[2]);
}

inline vec3 operator-(const vec3 &u, const vec3 &v)
{
    return vec3(u.e[0] - v.e[0], u.e[1] - v.e[1], u.e[2] - v.e[2]);
}

inline vec3 operator*(const vec3 &u, const vec3 &v)
{
    return vec3(u.e[0] * v.e[0], u.e[1] * v.e[1], u.e[2] * v.e[2]);
}

inline vec3 operator*(double t, const vec3 &v)
{
    return vec3(t*v.e[0], t*v.e[1], t*v.e[2]);
}

inline vec3 operator*(const vec3 &v, double t)
{
    return t * v;
}

inline vec3 operator/(vec3 v, double t)
{
    return (1/t) * v;
}

// Dot Product
// Crucial for lighting calculations like angle between vectors

inline double dot(const vec3 &u, const vec3 &v)
{
    return u.e[0] * v.e[0]
         + u.e[1] * v.e[1]
         + u.e[2] * v.e[2];
}

// Cross Product 
// Returns a vector perpendicular to both input vectors

inline vec3 cross(const vec3 &u, const vec3 &v)
{
    return vec3(u.e[1] * v.e[2] - u.e[2] * v.e[1],
                u.e[2] * v.e[0] - u.e[0] * v.e[2],
                u.e[0] * v.e[1] - u.e[1] * v.e[0]);
}

inline vec3 unit_vector(vec3 v)
{
    return v / v.length();
}


// Generate a random point inside a unit sphere
// We pick a random point in a unit cube and reject it if it's outside the sphere
// This method is also used to approch pi, it looks like a Monte Carlo Method

inline vec3 random_in_unit_sphere()
{
    while (true) {
        auto p = vec3::random(-1,1);
        if (p.length_squared() >= 1) continue;
        return p;
    }
}

// Generate a random unit vector i.e. a normalized vector
// Used for Lambertian distribution (True Lambertian)

inline vec3 random_unit_vector()
{
    return unit_vector(random_in_unit_sphere());
}

// Generate a random vector in the unit disk
// Used for Defocus Blur (Depth of Field)

inline vec3 random_in_unit_disk()
{
    while (true) {
        auto p = vec3(random_double(-1,1), random_double(-1,1), 0);
        if (p.length_squared() >= 1) continue;
        return p;
    }
}

// Reflect vector v around normal n

inline vec3 reflect(const vec3& v, const vec3& n)
{
    return v - 2*dot(v,n)*n;
}

// Refract vector uv through normal n with ratio etai_over_etat
// Uses Snell's Law

inline vec3 refract(const vec3& uv, const vec3& n, double etai_over_etat)
{
    auto cos_theta = fmin(dot(-uv, n), 1.0);
    vec3 r_out_perp =  etai_over_etat * (uv + cos_theta*n);
    vec3 r_out_parallel = -sqrt(fabs(1.0 - r_out_perp.length_squared())) * n;
    return r_out_perp + r_out_parallel;
}

#endif