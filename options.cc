#include <iostream>
#include <fstream>
#include <cmath>
#include "options.h"
#include "texture_image.h"

Options::Options(std::string read_file) {
    std::string keyword;
    MaterialColor curr_color;
    std::shared_ptr<TexImage> curr_tex;

    std::ifstream fh(read_file);
    if (fh.is_open()) {
        while (fh.good()) {
            keyword.clear();
            fh >> keyword;
            if (keyword != "f" && previous_was_face == true) {
                // Check if the last lines had been faces of a shape
                auto new_shape = std::make_shared<TriangleMesh>();
                new_shape->faces = tri_faces;
                new_shape->base_color = curr_color;
                new_shape->texture = curr_tex;
                shapes.emplace_back(new_shape);

                previous_was_face = false;
                tri_faces.clear();
            }
            if (keyword == "imsize") {
                fh >> width;
                fh >> height;
            }
            if (keyword == "eye") {          
                fh >> eye_pos[0];
                fh >> eye_pos[1];
                fh >> eye_pos[2];
            }
            if (keyword == "viewdir") {
                fh >> view_direc[0];
                fh >> view_direc[1];
                fh >> view_direc[2];
            }
            if (keyword == "hfov") {
                fh >> hfov;
                // Convert hfov to radians
                hfov = hfov * 3.1415926 / 180.0;
            }
            // if (keyword == "vfov") {
            //     double vfov;
            //     fh >> vfov;
            //     // Convert vfov to radians
            //     vfov = vfov * 3.1415926 / 180.0;
            //     hfov = 2.0 * std::atan(std::tan(vfov / 2.0) * (static_cast<double>(width) / static_cast<double>(height)));
            // }
            if (keyword == "updir") {
                fh >> up_direc[0];
                fh >> up_direc[1];
                fh >> up_direc[2];
            }
            if (keyword == "bkgcolor") {
                fh >> bg_color[0];
                fh >> bg_color[1];
                fh >> bg_color[2];

                fh >> bg_index_refrac;
            }
            if (keyword == "mtlcolor") {
                // mtlcolor Odr Odg Odb Osr Osg Osb ka kd ks n
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

                // Transparent and mirror-like surfaces
                fh >> curr_color.opacity;
                fh >> curr_color.index_refraction;
            }
            if (keyword == "sphere") { 
                auto new_shape = std::make_shared<Sphere>();
                fh >> new_shape->center[0];
                fh >> new_shape->center[1]; 
                fh >> new_shape->center[2];

                fh >> new_shape->radius;
                new_shape->base_color = curr_color;
                new_shape->texture = curr_tex;
                shapes.emplace_back(new_shape);
            }
            if (keyword == "v") {
                Point new_vertex;
                fh >> new_vertex[0];
                fh >> new_vertex[1];
                fh >> new_vertex[2];
                tri_verticies.push_back(new_vertex);

                // new_shape->base_color = curr_color;
                // shapes.emplace_back(new_shape);
            }
            if (keyword == "vn") {  // Vertex normal
                Vec new_norm;
                fh >> new_norm[0];
                fh >> new_norm[1];
                fh >> new_norm[2];
                tri_norms.push_back(new_norm);
            }
            if (keyword == "vt") {  // Vertex texture
                TexCoord new_tex;
                fh >> new_tex[0];
                fh >> new_tex[1];
                tri_textures.push_back(new_tex);
            }
            if (keyword == "f") {  // Triangle polygon face
                std::string p1;
                std::string p2;
                std::string p3;
                fh >> p1;
                fh >> p2;
                fh >> p3;

                std::string pts = p1 + " " + p2 + " "+ p3;
                unsigned int val1, val2, val3, val4, val5, val6, val7, val8, val9;
                Face new_face;

                // Check if syntax is  f v1/vt1/vn1 v2/vt2/vn2 v3/vt3/vn3
                // Smooth-shaded, textured
                if (sscanf(pts.c_str(), "%d/%d/%d %d/%d/%d %d/%d/%d", &val1, &val2, &val3, &val4, &val5, &val6, &val7, &val8, &val9) == 9) {
                    new_face.hasTex = true;
                    new_face.hasNorm = true;

                    new_face.p0 = tri_verticies.at(val1 - 1);
                    new_face.t0 = tri_textures.at(val2 - 1);
                    new_face.n0 = tri_norms.at(val3 - 1);
                    new_face.p1 = tri_verticies.at(val4 - 1);
                    new_face.t1 = tri_textures.at(val5 - 1);
                    new_face.n1 = tri_norms.at(val6 - 1);
                    new_face.p2 = tri_verticies.at(val7 - 1);
                    new_face.t2 = tri_textures.at(val8 - 1);
                    new_face.n2 = tri_norms.at(val9 - 1);
                }

                // Check if syntax is  f v1//vn1 v2//vn2 v3//vn3
                // Smooth-shaded, no texture
                else if (sscanf(pts.c_str(), "%d//%d %d//%d %d//%d", &val1, &val3, &val4, &val6, &val7, &val9) == 6) {
                    new_face.hasNorm = true;

                    new_face.p0 = tri_verticies.at(val1 - 1);
                    new_face.n0 = tri_norms.at(val3 - 1);
                    new_face.p1 = tri_verticies.at(val4 - 1);
                    new_face.n1 = tri_norms.at(val6 - 1);
                    new_face.p2 = tri_verticies.at(val7 - 1);
                    new_face.n2 = tri_norms.at(val9 - 1);
                }

                // Check if syntax is  f v1/vt1 v2/vt2 v3/vt3
                // No smooth shading, textured            
                else if (sscanf(pts.c_str(), "%d/%d %d/%d %d/%d", &val1, &val2, &val4, &val5, &val7, &val8) == 6) {
                    new_face.hasTex = true;
                    
                    new_face.p0 = tri_verticies.at(val1 - 1);
                    new_face.t0 = tri_textures.at(val2 - 1);
                    new_face.p1 = tri_verticies.at(val4 - 1);
                    new_face.t1 = tri_textures.at(val5 - 1);
                    new_face.p2 = tri_verticies.at(val7 - 1);
                    new_face.t2 = tri_textures.at(val8 - 1);
                }

                // Normal triangle -- no smooth shading, no texture
                else if (sscanf(pts.c_str(), "%d %d %d", &val1, &val4, &val7) == 3) {
                    new_face.p0 = tri_verticies.at(val1 - 1);
                    new_face.p1 = tri_verticies.at(val4 - 1);
                    new_face.p2 = tri_verticies.at(val7 - 1);
                }

                else {
                    std::cout << "Invalid format for triangle face\n";
                    continue;
                }
                // Ensure all faces of a mesh are being kept together
                previous_was_face = true;
                tri_faces.push_back(new_face);
            }
            if (keyword == "texture") {
                std::string file_name;
                fh >> file_name;
                curr_tex = std::make_shared<TexImage>(file_name, TexImage::Axis::Z);
            }
            if (keyword == "light") {
                // light x y z type (no intensity)
                Point direction;
                fh >> direction[0];
                fh >> direction[1];
                fh >> direction[2];

                int type = -1;
                fh >> type;
                if (type == 1) {
                    // Make point light
                    lights.emplace_back(std::make_shared<PointLight>(direction, 1));
                }
                else if (type == 0) {
                    // Make directional light
                    lights.emplace_back(std::make_shared<DirectionalLight>(direction, 1));
                }
                else {
                    std::cout << "Invalid light type provided in file";
                    break;
                }
                // *****TODO***** fix so works with light intensity
            }
            if (keyword == "attlight") {
                // Light attenuation
                // attlight x y z type i c1 c2 c3
                Point direction;
                fh >> direction[0];
                fh >> direction[1];
                fh >> direction[2];

                int type = -1;
                fh >> type;
                double intensity;
                fh >> intensity;
                double c1;
                double c2;
                double c3;
                fh >> c1;
                fh >> c2;
                fh >> c3;
                if (type == 1) {
                    // Make point light
                    lights.emplace_back(std::make_shared<PointLight>(direction, intensity, c1, c2, c3));
                }
                else if (type == 0) {
                    std::cout << "Directional lights can't do light attenuation\n";
                    break;
                    // lights.emplace_back(std::make_shared<DirectionalLight>(direction, 1));
                }
                else {
                    std::cout << "Invalid light type provided in file";
                    break;
                }
            }
            if (keyword == "depthcueing") { 
                // depthcueing dc-r dc-g dc-b alpha-min alpha-max dist-min dist-max
                DepthCue dc;
                fh >> dc.depth_color[0];
                fh >> dc.depth_color[1]; 
                fh >> dc.depth_color[2];

                fh >> dc.alpha_min;
                fh >> dc.alpha_max;
                fh >> dc.dist_near;
                fh >> dc.dist_far;

                depth_cue = dc;
            }
        }
    }
    else {
        std::cout << "Failed to open file" << std::endl;
    }

    fh.close();

    // Check if the last lines had been faces of a shape
    if (previous_was_face == true) {
        auto new_shape = std::make_shared<TriangleMesh>();
        new_shape->faces = tri_faces;
        new_shape->base_color = curr_color;
        new_shape->texture = curr_tex;
        shapes.emplace_back(new_shape);
        
        previous_was_face = false;
        tri_faces.clear();
    }

    // Decrease the intensity of the lights if there are multiple
    if (int size = lights.size(); size > 1) {
        double factor = 1.0 / size;
        for (auto& light : lights) {
            if (light->intensity != 1) {
                light->intensity *= factor;
            }
        }
    }
}