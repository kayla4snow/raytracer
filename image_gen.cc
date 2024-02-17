#include <iostream>
#include <fstream>
#include <string>
#include <algorithm>
#include <cmath>
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
        fprintf(fh, "%d %d %d ", static_cast<int>(color[0] * 255), static_cast<int>(color[1] * 255), static_cast<int>(color[2] * 255));
    }
}

Color ImageGen::blinn_phong(const Sphere& shape, const Point& intersect_pt, const Vec& ray) {  
    Color illumination = {0.0, 0.0, 0.0}; // Final value should be in range 0-1
    Vec diffuse_term = {0.0, 0.0, 0.0};
    Vec specular_term = {0.0, 0.0, 0.0};
    Vec N = shape.calc_normal_vector(intersect_pt);
    // std::cout << N[0] << " " << N[1] << " " << N[2] << std::endl;
    Color mult_lights = {0.0, 0.0, 0.0};

    // Flip ray back to original direction to fix bug (fix this in function call)
    Vec new_ray = scale_vec(-1, ray);

    Vec ambient_term = scale_vec(shape.base_color.coef_ambient, shape.base_color.diffuse_color);
    clamp_vec(ambient_term);

    for (auto& light : input.lights) {
        Vec L = light->calc_L(intersect_pt);
        Vec H = normalize_vec(add_vec(L, new_ray)); // Ray is V

        // Debugging 
        // if(abs(intersect_pt[0]) < 0.0069 && abs(intersect_pt[1] < 0.0069)){
        //     std::cout << "N: "<< N[0] << " " << N[1] << " " << N[2] << std::endl;
        //     std::cout << "L: "<< L[0] << " " << L[1] << " " << L[2] << std::endl;
        // }

        double dot_N_L = dot_product(N, L); 
        Vec temp1 = scale_vec(shape.base_color.coef_diffuse, shape.base_color.diffuse_color);
        diffuse_term = scale_vec(std::max(0.0, dot_N_L), temp1);
        clamp_vec(diffuse_term);

        // Debugging -- dot_N_H should never be above 1
        double dot_N_H = dot_product(N, H);
        // if(dot_N_H >= 0.99){
        //     std::cout << "uh oh" << std::endl;
        //     std::cout << N[0] << " " << N[1] << " " << N[2] << std::endl;
        //     std::cout << H[0] << " " << H[1] << " " << H[2] << std::endl;
        //     std::cout << dot_N_H << std::endl;
        // }

        Vec temp2 = scale_vec(shape.base_color.coef_specular, shape.base_color.spec_highlight);
        specular_term = scale_vec(std::pow(std::max(0.0, dot_N_H), shape.base_color.spec_exponent), temp2); 
        clamp_vec(specular_term);

        /*
        TODO
        - (switch shapes to shared pointer, like the lights are)
            - pass in shared pointer to shape in this function
            - if curr == shape, ignore
            - for (auto& shadowShape : input.shapes) {
                if (shape == shadowShape ) {
                    continue;
                }

                // call the shape->hit_test(L);
                // .... see below.
            }
        - for loop to iterate over input.shapes
            - skip if current shape
            - (move intersection stuff into shape class)
            - call hit_test(L)
                - use distance and point, and check if shape is between intersection_pt and light
            - (new equation for point lights to find distance from intersection_pt)

        */

        // Shadow flag
        double shadow_factor = 1.0;
        double x = shadow_factor * light->intensity;

        // For each successive light, combine diffuse and specular terms then add to mult_light
        mult_lights = add_vec(mult_lights, scale_vec(x, add_vec(diffuse_term, specular_term)));
    }

    illumination = add_vec(ambient_term, mult_lights);
    // Clamp illumination between 0-1
    clamp_vec(illumination);

    return illumination;
}

void ImageGen::compute_color(Vec ray) {
    double a = 0;
    double b = 0;
    double c = 0;
    bool found_anything = false;
    double shortest_dist = -1.0;
    Color color = input.bg_color; //TODO change to MaterialColor

    // Test
    //blinn_phong(input.shapes[0], {0.0, 0.0, -3.0}, {0.0, 0.0, 1.0});

    for (auto& shape : input.shapes) {
        // TODO move sphere equations to sphere class

        a = ray[0] * ray[0] + ray[1] * ray[1] + ray[2] * ray[2]; // Equals 1 if ray direction vec is normalized
        b = 2 * (ray[0] * (window.eye[0] - shape.center[0]) + ray[1] * (window.eye[1] - shape.center[1]) + ray[2] * (window.eye[2] - shape.center[2]));
        c = (window.eye[0] - shape.center[0]) * (window.eye[0] - shape.center[0]) + (window.eye[1] - shape.center[1]) * (window.eye[1] - shape.center[1]) + (window.eye[2] - shape.center[2]) * (window.eye[2] - shape.center[2]) - shape.radius * shape.radius;
        double new_discriminant = b * b - 4 * a * c;

        if (new_discriminant < 0) {
            // Skip current shape if discriminant is negative 
            continue;
        }

        double dist1 = (-b + sqrt(new_discriminant)) / (2 * a);
        double dist2 = (-b - sqrt(new_discriminant)) / (2 * a);
        double shape_dist = -10000;

        // Find shortest distance between dist1 and dist2
        if (dist1 >= 0.0 && dist1 < dist2) {
            shape_dist = dist1;
        }
        else if (dist2 >= 0.0 && dist2 < dist1) {
            shape_dist = dist2;
        }
        
        // Find intersection pt   TODO maybe need unnormalized ray
        Point intersection_pt = {window.eye[0] + shape_dist * ray[0], window.eye[1] + shape_dist * ray[1], window.eye[2] + shape_dist * ray[2]};
    
        if (shape_dist >= 0.0 && (shape_dist < shortest_dist || !found_anything)) { // Enforces range
            shortest_dist = shape_dist;
            // color = shape.base_color.diffuse_color; 
            // std::cout << intersection_pt[0]  << " " << intersection_pt[1] << " " << intersection_pt[2] << std::endl;
            // std::cout << ray[2] << std::endl;
            color = blinn_phong(shape, intersection_pt, scale_vec(-1.0, ray));
            // TODO compute shadows (and depth blending)
            found_anything = true;
        }

    }

    // TODO speed optimization: call Blinn phong to find color here instead in for loop

    color_to_ppm(color);
    // if (shortest_dist > 0.0) {
    //     // Ray pierces shape
    //     color_to_ppm(color);
    // }
    // else if (shortest_dist == 0.0) {
    //     // Ray grazes shape
    //     color_to_ppm(color);
    // }
    // else {
    //     // Ray misses shape
    //     color_to_ppm(input.bg_color);
    // }
}

void ImageGen::pixel_steps() { 
    Point curr_vert = window.ul;
    Vec horiz_step = step_size(window.ul, window.ur, input.width - 1);
    Vec vert_step = step_size(window.ul, window.ll, input.height - 1);
    for (int v_idx = 0; v_idx < input.height; ++v_idx) {
        // Track curr_vert separately and add a whole step, so that it begins again on the left edge of image
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