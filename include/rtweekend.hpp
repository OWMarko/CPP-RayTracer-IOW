#ifndef RTWEEKEND_H
#define RTWEEKEND_H

#include <cmath>
#include <limits>
#include <memory>
#include <cstdlib>

// We pull common types into the global namespace to make the code less verbose
// This allows us to write "shared_ptr" instead of "std::shared_ptr" for example

using std::shared_ptr;
using std::make_shared;
using std::sqrt;


// Define mathematical constants used for geometry and rendering limits

const double infinity = std::numeric_limits<double>::infinity();
const double pi = 3.1415926535897932385;

// Converts degrees to radians because C++ trigo functions expect radians but we think in degrees

inline double degrees_to_radians(double degrees)
{
    return degrees * pi / 180.0;
}

// Returns a random real number in [0, 1[, used for Monte Carlo 

inline double random_double()
{
    return rand() / (RAND_MAX + 1.0);
}

// Returns a random real number in [min, max[

inline double random_double(double min, double max)
{
    return min + (max-min)*random_double();
}

// Clamps the value x to the range [min, max]
// Essential for color processing to ensure we don't write values > 255 or < 0

inline double clamp(double x, double min, double max)
{
    if (x < min) return min;
    if (x > max) return max;
    return x;
}

#endif