#include <iostream>
#include <string>
#include <vector>

#include "image_gen.h"
#include "options.h"
#include "view_window.h"

int main(int argc, char *argv[]) {
    // Check if file was provided in command line
    if (argc < 2) {
        std::cout << "No file provided." << std::endl;
        return -1;
    }
    if (argc > 2) {
        std::cout << "Too many arguments provided." << std::endl;
        return -1;
    }

    std::string file_name = argv[1];

    // Open file found in command line or catch invalid file
    FILE *file_handle = fopen(file_name.c_str(), "r");
    if (file_handle == NULL) {
        std::cout << "Failed to open command line file" << std::endl;
        return -1;
    }
    
    // Edit file_name to end in .ppm
    std::string new_file_name = file_name;
    int index = file_name.find('.');
    new_file_name.replace(index + 1, 3, "ppm");

    Options input_options = Options(file_name);
    Window view_win = Window(input_options);
    ImageGen ppm_img = ImageGen(input_options, view_win); 
    ppm_img.create_ppm(new_file_name);

    fclose(file_handle);
    return 0;
}