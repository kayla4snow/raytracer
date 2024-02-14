#pragma once

#include <array>
#include <string>
#include <vector>
#include "ray_math.h"

// Color of shapes and background
struct Color {
    double red = 0.0; 
    double green = 0.0;
    double blue = 0.0;
};

// Shapes in the scene
struct Shape {
    // Sphere 
    Point s_pos = {0, 0, 0};
    double radius = 0.0;
    Color mat;
};

// Read options from a file that describe a scene
struct Options {
    // Explicit constructor 
    explicit Options(std::string file_name);

    // imsize w h
    int width = 0;
    int height = 0;
    // Eye x y z
    Point eye_pos = {0.0, 0.0, 0.0};
    // Viewdir x y z
    Vec view_direc = {0.0, 0.0, 0.0};
    // Hfov degrees
    double hfov = 0.0;
    // Updir x y z
    Vec up_direc = {0.0, 0.0, 0.0};
    // Bkgcolor r g b
    Color bg_color;

    // Shapes
    std::vector<Shape> shapes;
};