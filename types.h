#pragma once

#include <array>

using Vec = std::array<double, 3>;
using Point = std::array<double, 3>;
using Color = std::array<double, 3>;

struct Ray{
    Point origin;
    Vec direction;
}; 

// Color of shapes and background
struct MaterialColor {
    // Diffuse color, the intrinsic color of the object (without shadows/light)
    Color diffuse_color = {0.0, 0.0, 0.0};

    // Specular highlight
    Color spec_highlight = {0.0, 0.0, 0.0};

    // Reflectivity
    double coef_ambient = 0.0;
    double coef_diffuse = 0.0;
    double coef_specular = 0.0;
    
    // Specular exponent
    double spec_exponent = 0.0;
};

// TODO have a different color type for bg color, which doesnt have the other types?
// or just have the other variables set to 0? 