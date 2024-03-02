#include "texture_image.h"

TexImage::TexImage(std::string file_name, Axis axis) : _axis(axis){

}

TexImage::Axis TexImage::getAxis() const {
    return _axis;
}

Color TexImage::getPixelColor(double u, double v) {
    return {u, 0, v};
}