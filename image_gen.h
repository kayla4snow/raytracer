#ifndef IMAGE_GEN_H_
#define IMAGE_GEN_H_
#include <iostream>
#include <string>
#include "options.h"
#include "view_window.h"

// Generate a ppm file 
class ImageGen {
    public:       
        ImageGen(Options in_input, Window in_window);
        ~ImageGen();

        void create_ppm(std::string write_file);
        void color_to_ppm(Color color);
        void compute_color(Vec ray);
        void pixel_steps();

    private:
        FILE *fh = nullptr;

        Options input;
        Window window;
};


#endif // IMAGE_GEN_H_