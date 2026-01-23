#ifndef HITTABLE_LIST_H
#define HITTABLE_LIST_H

#include "hittable.hpp"

#include <memory>
#include <vector>

using std::shared_ptr;
using std::make_shared;

// It representing a list of hittable objects 
// It inherits from hittable itself meaning a list of objects is functionally treated 
// like a single object. It's a powerful Composite Design Pattern

class hittable_list : public hittable {
    public:
        // The container storing our objects (spheres, etc.)
        std::vector<shared_ptr<hittable>> objects;

    public:
        // Constructors
        hittable_list() {}
        hittable_list(shared_ptr<hittable> object) { add(object); }

        void clear() { objects.clear(); }
        void add(shared_ptr<hittable> object) { objects.push_back(object); }

        // We iterate through all objects in the list to see if the ray hits any of them
        // And we must keep track of the closest hit noted closest_so_far,
        // because we only see the object in front, not the ones hidden behind
        
        virtual bool hit(const ray& r, double t_min, double t_max, hit_record& rec) const override
        {
            hit_record temp_rec;
            bool hit_anything = false;
            auto closest_so_far = t_max;

            for (const auto& object : objects)
            {
                // We check hit against closest_so_far and effectively shrinking the search range every time we find a closer object
                if (object->hit(r, t_min, closest_so_far, temp_rec))
                {
                    hit_anything = true;
                    closest_so_far = temp_rec.t;
                    rec = temp_rec;
                }
            }

            return hit_anything;
        }
};

#endif