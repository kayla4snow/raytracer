#include <iostream>
#include "3d_object.h"
#include "view_window.h"
#include "options.h"

std::optional<HitResult> Sphere::hit_test(const Ray& ray) {
    return {};

    // double a = 0;
    // double b = 0;
    // double c = 0;
    // bool found_anything = false;
    // double shortest_dist = -1.0;
    // Color color = input.bg_color;

    // for (auto& shape : input.shapes) {
    //     // TODO use Ray or Vec here? Not sure if need both origin and direc from Ray

    //     a = ray[0] * ray[0] + ray[1] * ray[1] + ray[2] * ray[2]; // Equals 1 if ray direction vec is normalized
    //     b = 2 * (ray[0] * (window.eye[0] - shape.center[0]) + ray[1] * (window.eye[1] - shape.center[1]) + ray[2] * (window.eye[2] - shape.center[2]));
    //     c = (window.eye[0] - shape.center[0]) * (window.eye[0] - shape.center[0]) + (window.eye[1] - shape.center[1]) * (window.eye[1] - shape.center[1]) + (window.eye[2] - shape.center[2]) * (window.eye[2] - shape.center[2]) - shape.radius * shape.radius;
    //     double new_discriminant = b * b - 4 * a * c;

    //     if (new_discriminant < 0) {
    //         // Skip current shape if discriminant is negative 
    //         continue;
    //     }

    //     double pos_dist = (-b + sqrt(new_discriminant)) / (2 * a);
    //     double neg_dist = (-b - sqrt(new_discriminant)) / (2 * a);
    
    //     if (pos_dist >= 0.0 && (pos_dist < shortest_dist || !found_anything)) { // Enforces range
    //         shortest_dist = pos_dist;
    //         color = shape.base_color;
    //         found_anything = true;
    //     }

    //     if (neg_dist >= 0.0 && (neg_dist < shortest_dist || !found_anything)) { // Enforces range
    //         shortest_dist = neg_dist;
    //         color = shape.base_color;
    //         found_anything = true;
    //     }

    // }

    // // TODO maybe dont need this? 

    // // if (shortest_dist > 0.0) {
    // //     // Ray pierces shape
    // //     color_to_ppm(color);
    // // }
    // // else if (shortest_dist == 0.0) {
    // //     // Ray grazes shape
    // //     color_to_ppm(color);
    // // }
    // // else {
    // //     // Ray misses shape
    // //     color_to_ppm(input.bg_color);
    // // }

    // return std::make_pair{shortest_dist, color};
}

// Calculate the normal vector at a point on a sphere
Vec Sphere::calc_normal_vector(const Point& point) const {
    // N = [(xi, yi, zi) - (xc, yc, zc)] / r 
    // xi is ray/sphere intersection and xc is sphere center

    Vec temp = subtract_vec(point, center);
    return scale_vec(1.0 / radius, temp);
}