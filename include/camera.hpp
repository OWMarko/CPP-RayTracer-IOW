#ifndef CAMERA_H
#define CAMERA_H

#include "rtweekend.hpp"

// This class represent a virtual camera
// It handles the projection of the 3D world onto the 2D image plane (Sensor)
// It supports various things like :
// Positionable orientation (lookfrom, lookat)
// Adjustable Field of View (vfov)
// Depth of Field (Defocus Blur) simulating a real physical lens with an aperture

class camera {
    private:
        point3 origin;
        point3 lower_left_corner;
        vec3 horizontal;
        vec3 vertical;
        vec3 u, v, w;       // Camera frame orthonormal basis
        double lens_radius;

    public:
        camera(
            point3 lookfrom, // Camera position
            point3 lookat, // Where the camera is pointing
            vec3 vup, // The up direction for the camera (0,1,0)
            double vfov, // Vertical Field of View in degrees
            double aspect_ratio, // Width by Height
            double aperture, // Diameter of the virtual lens (0 = perfect focus)
            double focus_dist // Distance to the plane of perfect focus
        ) 
        {
            auto theta = degrees_to_radians(vfov);
            auto h = tan(theta/2);
            auto viewport_height = 2.0 * h;
            auto viewport_width = aspect_ratio * viewport_height;

            // Calculate the orthonormal basis for the camera orientation
            w = unit_vector(lookfrom - lookat); // Vector pointing opposite to view direction
            u = unit_vector(cross(vup, w));     // Vector pointing to the right
            v = cross(w, u);                    // Vector pointing up relative to camera

            origin = lookfrom;
            horizontal = focus_dist * viewport_width * u;
            vertical = focus_dist * viewport_height * v;
            lower_left_corner = origin - horizontal/2 - vertical/2 - focus_dist*w;

            lens_radius = aperture / 2;
        }

        // Generate a ray from the camera through pixel coordinates (s, t)
        // If aperture > 0 we originate the ray from a random point on the lens disk
        // instead of the exact center, this creates the depth of field effect
        
        ray get_ray(double s, double t) const
        {
            vec3 rd = lens_radius * random_in_unit_disk();
            vec3 offset = u * rd.x() + v * rd.y();

            return ray(
                origin + offset,
                lower_left_corner + s*horizontal + t*vertical - origin - offset
            );
        }
};

#endif