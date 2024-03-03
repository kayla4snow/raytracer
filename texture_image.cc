#include <stdexcept>
#include <iostream>
#include <fstream>
#include <string>
#include <cmath>
#include "texture_image.h"

TexImage::TexImage(std::string file_name, Axis axis) : _axis(axis){
    // Open file or catches invalid file
    std::ifstream fh(file_name);
    if (!fh.is_open()) {
        throw std::runtime_error(std::string("Unknown texture file: ").append(file_name));
    }

    // Check header strings
    std::string s_val;
    fh >> s_val;
    if (s_val != "P3") {
        throw std::runtime_error("Invalid texture file format");
    }
    fh >> width;
    fh >> height;

    double max_color;
    fh >> max_color;
    if (max_color <= 0 || width == 0 || height == 0) {
        throw std::runtime_error("Invalid texture file format");
    }

    int num_pixels = width * height;
    pixels.reserve(num_pixels);

    
    // Iterate through rest of file
    while (fh.good()) {
        Color new_color;
        fh >> new_color[0];
        fh >> new_color[1];
        fh >> new_color[2];

        new_color[0] /= max_color;
        new_color[1] /= max_color;
        new_color[2] /= max_color;

        pixels.emplace_back(new_color);
        if (num_pixels == pixels.size()) {
            break;
        }
    }

    fh.close();
}


TexImage::Axis TexImage::getAxis() const {
    return _axis;
}

Color TexImage::getPixelColor(double u, double v) {
    // unsigned int x = static_cast<unsigned int>(u * width);
    // unsigned int y = static_cast<unsigned int>(v * height);
    double ignore;
    unsigned int x = std::round(std::modf(u, &ignore) * (width - 1));
    unsigned int y = std::round(std::modf(v, &ignore) * (height - 1));

    unsigned int index = width * y + x;
    if (index < pixels.size()) {
        return pixels.at(index);
    }

    // Default 
    return {u, 0, v};
}