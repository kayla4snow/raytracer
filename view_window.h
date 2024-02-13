#pragma once

#include "options.h"
#include "ray_math.h"

// Create a viewing window
struct Window {
    public:
        explicit Window(Options input);

        double win_width = 0.0;
        double win_height = 0.0;
        // Distance to the view window
        double dist = 5.0; //TODO arbitrary value? How far away is the actaul picture?

        // Variables from input file
        double in_hfov = 0.0; 
        double pic_width = 0.0;
        double pic_height = 0.0;
        Point eye;
        Vec view_dir;
        Vec up_dir;

        // u and v are normalized
        Vec u;
        Vec v;

        // Corners of the window
        Point ul;
        Point ur;
        Point ll;
        Point lr;

    private:
        void compute_width();
        void compute_height();
        void compute_u_and_v();
        void compute_corners();
};
