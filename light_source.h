#pragma once
#include "types.h"

class Light {
    protected:
        Light();

        Point center = {0, 0, 0};
};

// Children

class PointLight : public Light {
    public:
        PointLight();

};

class DirectionalLight : public Light {
    public:
        DirectionalLight();
};