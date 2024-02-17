#pragma once
#include "types.h"

class Light {
    public:
        Light(double inIntensity) : intensity(inIntensity) {
        }
        virtual Vec calc_L(const Point& intersection) = 0;
        virtual double find_distance(const Point& point) = 0;
        virtual double attenuation_intensity(double distance) {
            return intensity;
        }

        double intensity = 0.5;  // Range 0-1
};


// PointLight is located at a position and emits light in all directions
class PointLight : public Light {
    public:
        // Normal constructor
        PointLight(Point inPos, double inIntensity);
        // Light attenuation version
        PointLight(Point inPos, double inIntensity, double c1, double c2, double c3);

        Vec calc_L(const Point& intersection) override;
        double find_distance(const Point& point) override;
        double attenuation_intensity(double distance) override;

        const Point position = {0, 0, 0};
        double c1 = 1.0;
        double c2 = 0.0;
        double c3 = 0.0;
};

// DirectionalLight does not have a position and emits light in a single direction
class DirectionalLight : public Light {
    public:
        DirectionalLight(Vec direc_input, double inIntensity);

        Vec calc_L(const Point& intersection) override;
        double find_distance(const Point& point) override;

        const Vec direction = {0, 0, 0};
        Vec L = {0, 0, 0};
};