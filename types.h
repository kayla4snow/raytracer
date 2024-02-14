#pragma once

#include <array>

using Vec = std::array<double, 3>;
using Point = std::array<double, 3>;

struct Ray{
    Point origin;
    Vec direction;
}; 

// Color of shapes and background
struct Color {
    double red = 0.0; 
    double green = 0.0;
    double blue = 0.0;
};
