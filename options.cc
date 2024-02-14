#include <iostream>
#include <fstream>
#include "options.h"

Options::Options(std::string read_file) {
    std::string keyword;
    Color curr_color;

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
                fh >> bg_color.red;
                fh >> bg_color.green;
                fh >> bg_color.blue;
            }
            if (keyword == "mtlcolor") {
                fh >> curr_color.red;
                fh >> curr_color.green;
                fh >> curr_color.blue;
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
        }
    }
    else {
        std::cout << "Failed to open file" << std::endl;
    }

    fh.close();
}