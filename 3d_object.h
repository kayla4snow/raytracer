#pragma once

#include <optional>
#include <vector>
#include "types.h"

// using HitResult = std::pair<double, Point>;
struct HitResult {
    double shape_dist = -100000;
    Point intersect_pt;
    Vec normal_vec;
};

class SceneObject {
    public:
        // Determine if there's an intersection between the ray passed in and this shape
        virtual std::optional<HitResult> hit_test(const Ray& ray) = 0;

        Point center = {0, 0, 0};
        MaterialColor base_color;
};

class Sphere : public SceneObject {
    public:
        std::optional<HitResult> hit_test(const Ray& ray) override;

        double radius = 0.0;
};

class TriangleMesh : public SceneObject {
    public:
        std::optional<HitResult> hit_test(const Ray& ray) override;

        // List of faces
        std::vector<Face> faces;
};