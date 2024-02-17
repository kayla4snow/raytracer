#pragma once

#include <optional>
#include "types.h"

using HitResult = std::pair<double, Point>;

class SceneObject {
    public:
        virtual std::optional<HitResult> hit_test(const Ray& ray) = 0; 
        virtual Vec calc_normal_vector(const Point& point) const = 0;

        Point center = {0, 0, 0};
        MaterialColor base_color;
};

class Sphere : public SceneObject {
    public:
        std::optional<HitResult> hit_test(const Ray& ray) override;
        Vec calc_normal_vector(const Point& point) const override;

        double radius = 0.0;
};