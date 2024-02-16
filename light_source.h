#pragma once
#include "types.h"

class Light {
    public:
        Light(double inIntensity) : intensity(inIntensity) {
        }
        virtual Vec calc_L(const Point& intersection) = 0;

        double intensity = 0.5;  // Range 0-1
};


// PointLight is located at a position and emits light in all directions
class PointLight : public Light {
    public:
        PointLight(Point inPos, double inIntensity);
        Vec calc_L(const Point& intersection) override;

        const Point position = {0, 0, 0};
};

// DirectionalLight does not have a position and emits light in a single direction
class DirectionalLight : public Light {
    public:
        DirectionalLight(Vec direc_input, double inIntensity);
        Vec calc_L(const Point& intersection) override;

        const Vec direction = {0, 0, 0};
        Vec L = {0, 0, 0};
};