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
    if (fh == nullptr) {
        std::cout << "Failed to open file\n" << std::endl;
        return;
    }

    // Format for the header
    fprintf(fh, "P3\n%d %d\n255\n", input.width, input.height);

    // Call pixel_steps to fill in rest of file
    pixel_steps();
}

void ImageGen::color_to_ppm(Color color) {
    if (fh) {
        // Write color to ppm file after mult by 255
        fprintf(fh, "%d %d %d ", static_cast<int>(color[0] * 255), static_cast<int>(color[1] * 255), static_cast<int>(color[2] * 255));
    }
}

Color ImageGen::blinn_phong(std::shared_ptr<SceneObject> shape, const HitResult& hit_result, const Vec& ray) {  
    Color illumination = {0.0, 0.0, 0.0}; // Final value should be in range 0-1
    Vec diffuse_term = {0.0, 0.0, 0.0};
    Vec specular_term = {0.0, 0.0, 0.0};
    Vec N = hit_result.normal_vec;

    Color mult_lights = {0.0, 0.0, 0.0};
    MaterialColor color = shape->tex_color(hit_result);

    Vec ambient_term = scale_vec(color.coef_ambient, color.diffuse_color);
    clamp_vec(ambient_term);

    for (auto& light : input.lights) {
        Vec L = light->calc_L(hit_result.intersect_pt);
        Vec H = normalize_vec(add_vec(L, ray)); // Ray is V

        double dot_N_L = dot_product(N, L); 
        Vec temp1 = scale_vec(color.coef_diffuse, color.diffuse_color);
        diffuse_term = scale_vec(std::max(0.0, dot_N_L), temp1);
        clamp_vec(diffuse_term);

        double dot_N_H = dot_product(N, H);

        Vec temp2 = scale_vec(color.coef_specular, color.spec_highlight);
        specular_term = scale_vec(std::pow(std::max(0.0, dot_N_H), color.spec_exponent), temp2); 
        clamp_vec(specular_term);

        // Shadow ray
        Ray shadow_ray = {hit_result.intersect_pt, L};
        double distance_to_light = light->find_distance(hit_result.intersect_pt);

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
            if (shadow_hit->shape_dist < 0 || shadow_hit->shape_dist > distance_to_light) {
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
    Color color = input.bg_color; 

    for (auto& shape : input.shapes) {
        // Call hit_test (in 3d_object) to determine if there's an intersection
        auto hit_result = shape->hit_test(ray);

        if (!hit_result) {
            // If hit_test doesn't return anything (no contact with shape), skip this shape
            continue;
        }

        if (hit_result->shape_dist >= 0.0 && (hit_result->shape_dist < shortest_dist || !found_anything)) { // Enforces range
            shortest_dist = hit_result->shape_dist;

            color = blinn_phong(shape, *hit_result, scale_vec(-1, ray.direction));
            if (input.depth_cue) {
                color = depth_cueing(hit_result->shape_dist, color);
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