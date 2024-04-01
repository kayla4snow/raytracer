#pragma once

#include <array>
#include <memory>

using Vec = std::array<double, 3>;
using Point = std::array<double, 3>;
using Color = std::array<double, 3>;
// using Index3 = std::array<unsigned int, 3>;
using TexCoord = std::array<double, 2>;

struct Face {
    bool hasNorm = false;
    bool hasTex = false;

    Point p0, p1, p2;
    Vec n0, n1, n2;
    TexCoord t0, t1, t2;

    // Flat shading normal vector (constant per triangle)
    Vec norm_flat_shade;
};

struct Ray{
    Point origin;
    Vec direction;
}; 

// Forward declare TexImage
class TexImage;

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

    // Transparent and mirror-like surfaces
    double opacity = 1.0;  // alpha
    double index_refraction = 1.0;  // eta (the weird n), typically in range 1-2.5
};

// TODO have a different color type for bg color, which doesnt have the other types?
// or just have the other variables set to 0? 