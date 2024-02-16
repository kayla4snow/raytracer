#include <iostream>
#include <fstream>
#include "options.h"

Options::Options(std::string read_file) {
    std::string keyword;
    MaterialColor curr_color;

    std::ifstream fh(read_file);
    if (fh.is_open()) {
        while (fh.good()) {
            keyword.clear();
            fh >> keyword;
            if (keyword == "imsize") {
                fh >> width;
                fh >> height;

                fh >> keyword;
                if (keyword != "eye") {
                    std::cout << "File lists inputs in wrong order or missing" << std::endl;
                    return;
                }
                fh >> eye_pos[0];
                fh >> eye_pos[1];
                fh >> eye_pos[2];

                fh >> keyword;
                if (keyword != "viewdir") {
                    std::cout << "File lists inputs in wrong order or missing" << std::endl;
                    return;
                }
                fh >> view_direc[0];
                fh >> view_direc[1];
                fh >> view_direc[2];

                fh >> keyword;
                if (keyword != "hfov") {
                    std::cout << "File lists inputs in wrong order or missing" << std::endl;
                    return;
                }
                fh >> hfov;
                // Convert hfov to radians
                hfov = hfov * 3.1415926 / 180.0;
                
                fh >> keyword;
                if (keyword != "updir") {
                    std::cout << "File lists inputs in wrong order or missing" << std::endl;
                    return;
                }
                fh >> up_direc[0];
                fh >> up_direc[1];
                fh >> up_direc[2];

                fh >> keyword;
                if (keyword != "bkgcolor") {
                    std::cout << "File lists inputs in wrong order or missing" << std::endl;
                    return;
                }
                fh >> bg_color[0];
                fh >> bg_color[1];
                fh >> bg_color[2];
            }
            if (keyword == "mtlcolor") {
                // Diffuse color
                fh >> curr_color.diffuse_color[0];
                fh >> curr_color.diffuse_color[1];
                fh >> curr_color.diffuse_color[2];
                
                // Specular highlight
                fh >> curr_color.spec_highlight[0];
                fh >> curr_color.spec_highlight[1];
                fh >> curr_color.spec_highlight[2];

                // Reflectivity 
                fh >> curr_color.coef_ambient;
                fh >> curr_color.coef_diffuse;
                fh >> curr_color.coef_specular;

                // Specular exponent
                fh >> curr_color.spec_exponent;
            }
            if (keyword == "sphere") { 
                Sphere new_shape;
                fh >> new_shape.center[0];
                fh >> new_shape.center[1]; 
                fh >> new_shape.center[2];

                fh >> new_shape.radius;
                new_shape.base_color = curr_color;
                shapes.push_back(new_shape);
            }
            if (keyword == "light") {
                // Light new_light;
                Point direction;
                fh >> direction[0];
                fh >> direction[1];
                fh >> direction[2];
                // TODO make first three inputs variables, set to light later after determinung type

                int type = -1;
                fh >> type;
                if (type == 0) {
                    // Make point light
                    lights.emplace_back(std::make_shared<PointLight>(direction, 0.5));
                }
                else if (type == 1) {
                    // Make directional light
                    lights.emplace_back(std::make_shared<DirectionalLight>(direction, 0.5));
                }
                else {
                    std::cout << "Invalid light type provided in file";
                    break;
                }
            }
        }
    }
    else {
        std::cout << "Failed to open file" << std::endl;
    }

    fh.close();
}