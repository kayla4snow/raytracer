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
        void compute_color(const Ray& ray);
        void pixel_steps();

    private:
        Color blinn_phong(std::shared_ptr<SceneObject> shape, const Point& intersect_pt, const Vec& ray);
        
        FILE *fh = nullptr;

        Options input;
        Window window;
};


#endif // IMAGE_GEN_H_