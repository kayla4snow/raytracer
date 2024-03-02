#pragma once

#include <string>
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
        
        // read file
        // store file
        // width and height of file
};