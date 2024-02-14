// #pragma once
#ifndef RAY_MATH_H_
#define RAY_MATH_H_

#include <array>
#include "math.h"
#include "types.h"

// Scale
inline Vec scale_vec(double scale, const Vec& inVec) {
    return Vec{inVec[0] * scale, inVec[1] * scale, inVec[2] * scale};
}

// Addition 
inline Vec add_vec(const Vec& vec1, const Vec& vec2) {
    return Vec{vec1[0] + vec2[0], vec1[1] + vec2[1], vec1[2] + vec2[2]};
}

// Subtraction
inline Vec subtract_vec(const Vec& vec1, const Vec& vec2) {
    return Vec{vec1[0] - vec2[0], vec1[1] - vec2[1], vec1[2] - vec2[2]};
}

// Magnitude 
inline double magnitude_vec(const Vec& vec) {
    // Find distance of 3D vector (length of original vector)
    return sqrt(vec[0] * vec[0] + vec[1] * vec[1] + vec[2] * vec[2]);
}
// Normalize
inline Vec normalize_vec(const Vec& vec) {
    double scaling = magnitude_vec(vec);
    if (scaling != 0) {
        // Divide by scaled vector 
        return Vec{vec[0] / scaling, vec[1] / scaling, vec[2] / scaling};
    } 
    return Vec{0, 0, 0};
}

// Cross product 
inline Vec cross_product(const Vec& vec1, const Vec& vec2) {
    double i = vec1[1] * vec2[2] - vec1[2] * vec2[1];
    double j = vec1[0] * vec2[2] - vec1[2] * vec2[0];
    double k = vec1[0] * vec2[1] - vec1[1] * vec2[0];
    return Vec{i, -j, k}; 
}

// Parametric representation of a ray
inline Vec compute_ray(const Vec& origin, const Vec& target) {
    Vec subtracted = subtract_vec(target, origin);
    return normalize_vec(subtracted);
}

inline Vec step_size(const Vec& pt1, const Vec& pt2, int num_steps) {
    Vec vec = subtract_vec(pt2, pt1);
    return Vec{vec[0] / num_steps, vec[1] / num_steps, vec[2]/ num_steps};
}

#endif // RAY_MATH_H_