#pragma once

#include <string>
#include <vector>
#include "types.h"

class TexImage {
    public:
        enum class Axis {
            X,
            Y,
            Z
        };

        TexImage(std::string file_name, Axis axis);

        Axis getAxis() const;
        Color getPixelColor(double u, double v);
    
    private:
        Axis _axis;
        unsigned int width;
        unsigned int height;
        
        std::vector<Color> pixels;
};