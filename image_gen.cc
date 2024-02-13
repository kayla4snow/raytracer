#include <iostream>
#include <fstream>
#include <string>
#include "image_gen.h"

ImageGen::ImageGen(Options in_input, Window in_window) : input(in_input), window(in_window) {
}

ImageGen::~ImageGen() {
    if (fh) {
        fclose(fh);
    }
}

void ImageGen::create_ppm(std::string write_file) {
    // Creates new file or catches invalid file
    fh = fopen(write_file.c_str(), "w");
    if (fh == NULL) {
        std::cout << "Failed to open file\n" << std::endl;
        return;
    }

    // Format for the header
    fprintf(fh, "P3\n%d %d\n255\n", input.width, input.height);

    // Test code
    //Vec ray = compute_ray({0, 0, 0}, {0, 0, -1}); // Compute ray with eye as the origin
    //ray = normalize_vec(ray);
    //compute_color(ray);

    // Call pixel_steps to fill in rest of file
    pixel_steps();
}

void ImageGen::color_to_ppm(Color color) {
    if (fh) {
        // Write color to ppm file after mult by 255
        fprintf(fh, "%d %d %d ", static_cast<int>(color.red * 255), static_cast<int>(color.green * 255), static_cast<int>(color.blue * 255));
    }
}

void ImageGen::compute_color(Vec ray) {
    double a = 0;
    double b = 0;
    double c = 0;
    bool found_anything = false;
    double discriminant = -1.0;
    Color color = input.bg_color;

    for (auto& shape : input.shapes) {
        a = ray[0] * ray[0] + ray[1] * ray[1] + ray[2] * ray[2]; // Equals 1 if ray direction vec is normalized
        b = 2 * (ray[0] * (window.eye[0] - shape.s_pos[0]) + ray[1] * (window.eye[1] - shape.s_pos[1]) + ray[2] * (window.eye[2] - shape.s_pos[2]));
        c = (window.eye[0] - shape.s_pos[0]) * (window.eye[0] - shape.s_pos[0]) + (window.eye[1] - shape.s_pos[1]) * (window.eye[1] - shape.s_pos[1]) + (window.eye[2] - shape.s_pos[2]) * (window.eye[2] - shape.s_pos[2]) - shape.radius * shape.radius;
        double new_discriminant = b * b - 4 * a * c;

        if (new_discriminant >= 0.0) {
            if (!found_anything || new_discriminant < discriminant) {
                discriminant = new_discriminant;
                color = shape.mat;
                found_anything = true;
            }
        }
    }

    if (discriminant > 0) {
        // Ray pierces shape
        color_to_ppm(color);
    }
    else if (discriminant == 0.0) {
        // Ray grazes shape
        color_to_ppm(color);
    }
    else {
        // Ray misses shape
        color_to_ppm(input.bg_color);
    }
}

void ImageGen::pixel_steps() { 
    Point curr_vert = window.ul;
    Vec horiz_step = step_size(window.ul, window.ur, input.width - 1);
    Vec vert_step = step_size(window.ul, window.ll, input.height - 1);
    for (int v_idx = 0; v_idx < input.height; ++v_idx) {
        // Track curr_vert separately and adding a whole step, so that it begins again on the left edge of image
        Point curr = curr_vert; // Start next row

        for (int h_idx = 0; h_idx < input.width; ++h_idx) {
            Vec ray = compute_ray(window.eye, curr); // Compute ray with eye as the origin
            ray = normalize_vec(ray);
            // Call compute_color on each ray (in each iteration)
            compute_color(ray);
            curr = add_vec(curr, horiz_step); 
        }
        curr_vert = add_vec(curr_vert, vert_step);
        // Add newline in output file at the end of every horizontal row
        if (fh) {
            fprintf(fh, "\n");
        }
    }
}