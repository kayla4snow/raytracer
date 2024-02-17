#pragma once

#include <array>
#include <memory>
#include <string>
#include <vector>
// #include "ray_math.h"
#include "types.h"
#include "3d_object.h"
#include "light_source.h"

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

    // Shapes and lights
    // TODO move somewhere else
    std::vector<std::shared_ptr<SceneObject>> shapes; 
    std::vector<std::shared_ptr<Light>> lights;
};