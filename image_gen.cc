#include <iostream>
#include <fstream>
#include <string>
#include <algorithm>
#include <cmath>
#include <iomanip>
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

Color ImageGen::blinn_phong(std::shared_ptr<SceneObject> shape, const Point& intersect_pt, const Vec& ray) {  
    Color illumination = {0.0, 0.0, 0.0}; // Final value should be in range 0-1
    Vec diffuse_term = {0.0, 0.0, 0.0};
    Vec specular_term = {0.0, 0.0, 0.0};
    Vec N = shape->calc_normal_vector(intersect_pt);

    // // Flip ray back to original direction to fix bug (fix this in function call)
    // Vec new_ray = scale_vec(-1, ray);

    // Debugging
    // bool extraDebug = false;
    // if(abs(intersect_pt[0]) < 0.0069 && abs((intersect_pt[1] - 0.93) < 0.0069)){
    //     extraDebug = true;
    // }
    // if (extraDebug) {
    //     std::cout << std::fixed << std::setprecision(6);
    //     std::cout << "n: " << N[0] << " " << N[1] << " " << N[2] << std::endl;
    //     std::cout << "r: " << ray[0] << " " << ray[1] << " " << ray[2] << std::endl;
    //     std::cout << "p: " << intersect_pt[0] << " " << intersect_pt[1] << " " << intersect_pt[2] << std::endl << std::endl;
    // }

    Color mult_lights = {0.0, 0.0, 0.0};

    Vec ambient_term = scale_vec(shape->base_color.coef_ambient, shape->base_color.diffuse_color);
    clamp_vec(ambient_term);

    for (auto& light : input.lights) {
        Vec L = light->calc_L(intersect_pt);
        Vec H = normalize_vec(add_vec(L, ray)); // Ray is V

        // Debugging 
        // if (extraDebug) {
        //     std::cout << "N: "<< N[0] << " " << N[1] << " " << N[2] << std::endl;
        //     std::cout << "H: "<< H[0] << " " << H[1] << " " << H[2] << std::endl;
        //     std::cout << "L: "<< L[0] << " " << L[1] << " " << L[2] << std::endl << std::endl;
        // }

        double dot_N_L = dot_product(N, L); 
        Vec temp1 = scale_vec(shape->base_color.coef_diffuse, shape->base_color.diffuse_color);
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

        Vec temp2 = scale_vec(shape->base_color.coef_specular, shape->base_color.spec_highlight);
        specular_term = scale_vec(std::pow(std::max(0.0, dot_N_H), shape->base_color.spec_exponent), temp2); 
        // specular_term = scale_vec(10000, specular_term);
        clamp_vec(specular_term);
        // std::cout << "Specular exp: " << shape->base_color.spec_exponent << std::endl;
        // if (specular_term[0] > 0 || specular_term[1] > 0 || specular_term[2] > 0) {
        //     std::cout << "Specular term: " << specular_term[0] << " " << specular_term[1] << " " << specular_term[2] << std::endl;
        // }
        // std::cout << dot_N_H << std::endl;

        // Shadow ray
        Ray shadow_ray = {intersect_pt, L};
        double distance_to_light = light->find_distance(intersect_pt);

        // Shadow flag
        double shadow_factor = 1.0;
        for (auto& shadowShape : input.shapes) {
            if (shape.get() == shadowShape.get()) {
                // Compare the pointers of the passed in shape and the current shape (iterating the whole list)
                continue;
            }

            auto shadow_hit = shadowShape->hit_test(shadow_ray);
            // Pass if no shapes in between
            if (!shadow_hit) {
                continue;
            }
            // Check if shape is behind light
            if (shadow_hit->first > distance_to_light) {
                continue;
            }
            shadow_factor = 0.0;
        }

        double x = shadow_factor * light->attenuation_intensity(distance_to_light);

        // For each successive light, combine diffuse and specular terms then add to mult_light
        mult_lights = add_vec(mult_lights, scale_vec(x, add_vec(diffuse_term, specular_term)));
    }

    illumination = add_vec(ambient_term, mult_lights);
    // Clamp illumination between 0-1
    clamp_vec(illumination);

    //Debugging
    // if (extraDebug) {
    //     std::cout << "c: "<< illumination[0] << " " << illumination[1] << " " << illumination[2] << std::endl << std::endl;

    // }
    return illumination;
}

Color ImageGen::depth_cueing(double shape_dist, Color illumination) {
    double alpha_depth;
    // Find alpha_depth using piece-wise function
    if (shape_dist <= input.depth_cue->dist_near) {
        // dist_near is closer to the eye
        alpha_depth = input.depth_cue->alpha_max;
    }
    else if (shape_dist >= input.depth_cue->dist_far) {
        // dist_far is farther from the eye
        alpha_depth = input.depth_cue->alpha_min;
    }
    else {
        alpha_depth = input.depth_cue->alpha_min + (input.depth_cue->alpha_max - input.depth_cue->alpha_min) * ((input.depth_cue->dist_far - shape_dist) / (input.depth_cue->dist_far - input.depth_cue->dist_near));
    }
    std::clamp(alpha_depth, 0.0, 1.0);

    // Depth cue equation, using illumination found in Blinn-Phong
    Vec depth_cue = add_vec(scale_vec(alpha_depth, illumination), 
                            scale_vec(1 - alpha_depth, input.depth_cue->depth_color));
    
    clamp_vec(depth_cue);
    return depth_cue;
}

void ImageGen::compute_color(const Ray& ray) {
    bool found_anything = false;
    double shortest_dist = -1.0;
    Color color = input.bg_color; //TODO change to MaterialColor

    for (auto& shape : input.shapes) {
        // Call hit_test (in 3d_object) to determine if there's an intersection
        auto hit_result = shape->hit_test(ray);

        if (!hit_result) {
            // If hit_test doesn't return anything (no contact with sphere), skip this shape
            continue;
        }
        auto& [shape_dist, intersection_pt] = *hit_result;

        if (shape_dist >= 0.0 && (shape_dist < shortest_dist || !found_anything)) { // Enforces range
            shortest_dist = shape_dist;

            color = blinn_phong(shape, intersection_pt, scale_vec(-1, ray.direction));
            if (input.depth_cue) {
                color = depth_cueing(shape_dist, color);
            }

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
            Ray ray = compute_ray(window.eye, curr); // Compute ray with eye as the origin

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