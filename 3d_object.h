#pragma once

#include "types.h"

class SceneObject {
    public:
        Point center = {0, 0, 0};
        Color base_color;

};

class Sphere : public SceneObject {
    public:
        double radius = 0.0;
};