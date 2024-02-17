#include <limits>

#include "light_source.h"
#include "ray_math.h"

// PointLight functions

PointLight::PointLight(Point inPos, double inIntensity) : position(inPos), Light(inIntensity) {
}

Vec PointLight::calc_L(const Point& intersection) {
    Vec newVec = subtract_vec(position, intersection);
    return normalize_vec(newVec);
}

// Find distance between light position and point passed in
double PointLight::find_distance(const Point& point) {
    return magnitude_vec(subtract_vec(position, point));
}


// DirectionalLight functions

DirectionalLight::DirectionalLight(Vec direc_input, double inIntensity) : direction(direc_input), Light(inIntensity) {
    L = normalize_vec(scale_vec(-1, direction));
    // L = normalize_vec(scale_vec(1, direction));
}

Vec DirectionalLight::calc_L(const Point& intersection) {
    return L;
}

// Directional Lights are infinitely far away
double DirectionalLight::find_distance(const Point& point) {
    return std::numeric_limits<double>::max();
}