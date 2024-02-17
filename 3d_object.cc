#include <iostream>
#include "3d_object.h"
#include "view_window.h"
#include "options.h"

std::optional<HitResult> Sphere::hit_test(const Ray& ray) {
    double a = ray.direction[0] * ray.direction[0] + ray.direction[1] * ray.direction[1] + ray.direction[2] * ray.direction[2]; // Equals 1 if ray direction vec is normalized
    double b = 2 * (ray.direction[0] * (ray.origin[0] - center[0]) + ray.direction[1] * (ray.origin[1] - center[1]) + ray.direction[2] * (ray.origin[2] - center[2]));
    double c = (ray.origin[0] - center[0]) * (ray.origin[0] - center[0]) + (ray.origin[1] - center[1]) * (ray.origin[1] - center[1]) + (ray.origin[2] - center[2]) * (ray.origin[2] - center[2]) - radius * radius;
    double new_discriminant = b * b - 4 * a * c;

    if (new_discriminant < 0) {
        // Skip current shape if discriminant is negative, return empty
        return {};
    }

    double dist1 = (-b + sqrt(new_discriminant)) / (2 * a);
    double dist2 = (-b - sqrt(new_discriminant)) / (2 * a);
    double shape_dist = -10000;

    // Find shortest distance between dist1 and dist2
    if (dist1 >= 0.0 && dist1 < dist2) {
        shape_dist = dist1;
    }
    else if (dist2 >= 0.0 && dist2 < dist1) {
        shape_dist = dist2;
    }
    
    // Find intersection pt   TODO maybe need unnormalized ray
    Point intersection_pt = {ray.origin[0] + shape_dist * ray.direction[0], ray.origin[1] + shape_dist * ray.direction[1], ray.origin[2] + shape_dist * ray.direction[2]};
    
    return HitResult{shape_dist, intersection_pt};
}

// Calculate the normal vector at a point on a sphere
Vec Sphere::calc_normal_vector(const Point& point) const {
    // N = [(xi, yi, zi) - (xc, yc, zc)] / r 
    // xi is ray/sphere intersection and xc is sphere center

    Vec temp = subtract_vec(point, center);
    return normalize_vec(scale_vec(1.0 / radius, temp));
    // Added normalization
}