#pragma once

#include <optional>
#include "types.h"

using HitResult = std::pair<double, Point>;

class SceneObject {
    public:
        // TODO change return type and add arguments 
        virtual std::optional<HitResult> hit_test(const Ray& ray) = 0; 

        Point center = {0, 0, 0};
        MaterialColor base_color;
};

class Sphere : public SceneObject {
    public:
        // TODO calc distance and color at that point here instead of in image_gen
        std::optional<HitResult> hit_test(const Ray& ray);
        Vec calc_normal_vector(const Point& point) const;

        double radius = 0.0;
};