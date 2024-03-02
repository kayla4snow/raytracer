#include <iostream>
#include <cmath>
#include "3d_object.h"
#include "view_window.h"
#include "options.h"
#include "texture_image.h"

const double PI = 3.141592654;

std::optional<HitResult> Sphere::hit_test(const Ray& ray) {
    double a = ray.direction[0] * ray.direction[0] + ray.direction[1] * ray.direction[1] + ray.direction[2] * ray.direction[2]; // Equals 1 if ray direction vec is normalized
    double b = 2 * (ray.direction[0] * (ray.origin[0] - center[0]) + ray.direction[1] * (ray.origin[1] - center[1]) + ray.direction[2] * (ray.origin[2] - center[2]));
    double c = (ray.origin[0] - center[0]) * (ray.origin[0] - center[0]) + (ray.origin[1] - center[1]) * (ray.origin[1] - center[1]) + (ray.origin[2] - center[2]) * (ray.origin[2] - center[2]) - radius * radius;
    double new_discriminant = b * b - 4 * a * c;

    if (new_discriminant < 0) {
        // Skip current shape if discriminant is negative, return empty
        return {};
    }

    HitResult retval; 

    double dist1 = (-b + sqrt(new_discriminant)) / (2 * a);
    double dist2 = (-b - sqrt(new_discriminant)) / (2 * a);

    // Find shortest distance between dist1 and dist2
    if (dist1 >= 0.0 && dist1 < dist2) {
        retval.shape_dist = dist1;
    }
    else if (dist2 >= 0.0 && dist2 < dist1) {
        retval.shape_dist = dist2;
    }
    
    // Find intersection pt   TODO maybe need unnormalized ray
    retval.intersect_pt = {ray.origin[0] + retval.shape_dist * ray.direction[0], ray.origin[1] + retval.shape_dist * ray.direction[1], ray.origin[2] + retval.shape_dist * ray.direction[2]};

    // Calculate the normal vector at a point on a sphere
    // N = [(xi, yi, zi) - (xc, yc, zc)] / r 
    // xi is ray/sphere intersection and xc is sphere center
    Vec temp = subtract_vec(retval.intersect_pt, center);
    retval.normal_vec = normalize_vec(scale_vec(1.0 / radius, temp));
    
    return retval;
}

MaterialColor Sphere::tex_color(const HitResult& hit_result) {
    MaterialColor color = base_color;
    if (!texture) {
        // Default is the base_color if no texture specified
        return color;
    }
    
    double phi;
    double theta;
    switch(texture->getAxis())
    {
        case TexImage::Axis::Z:
            phi = acos(hit_result.normal_vec[2]);
            theta = atan2(hit_result.normal_vec[1], hit_result.normal_vec[0]);
            break;
        case TexImage::Axis::Y:
            phi = acos(hit_result.normal_vec[1]);
            theta = atan2(hit_result.normal_vec[0], hit_result.normal_vec[2]);
            break;
        case TexImage::Axis::X:
            phi = acos(hit_result.normal_vec[0]);
            theta = atan2(hit_result.normal_vec[2], hit_result.normal_vec[1]);
            break;
    }



    double u;
    double v;
    if (theta > 0) {
        u = theta / (2 * PI);
    }
    else {
        u = (theta + 2 * PI) / (2 * PI);
    }
    if (phi > 0) {
        v = phi / (2 * PI);
    }
    else {
        v = (phi + 2 * PI) / (2 * PI);
    }

    color.diffuse_color = texture->getPixelColor(u, v);

    return color;
}


std::optional<HitResult> TriangleMesh::hit_test(const Ray& ray) {
    // Loop through all faces of this mesh shape
    for(auto& face : faces) {

        Vec edge1 = subtract_vec(face.p1, face.p0);
        Vec edge2 = subtract_vec(face.p2, face.p0);
        // The order of the cross product is important--there is a front and back of the triangle
        face.norm_flat_shade = cross_product(edge1, edge2);

        double a = face.norm_flat_shade[0];
        double b = face.norm_flat_shade[1];
        double c = face.norm_flat_shade[2];

        double denominator = a * ray.direction[0] + b * ray.direction[1] + c * ray.direction[2];
        if (denominator == 0.0) {
            // Skip if denominator is 0; the ray is parallel to the plane of the triangle
            continue;
        }

        double d = -1.0 * ((a * face.p0[0]) + (b * face.p0[1]) + (c * face.p0[2]));
        double numerator = -1.0 * (a * ray.origin[0] + b * ray.origin[1] + c * ray.origin[2] + d);
        double shape_dist = numerator / denominator;
        if (shape_dist < 0.0) {
            // Shape is behind eye
            continue;
        }

        HitResult retval;
        retval.shape_dist = shape_dist;
        retval.intersect_pt = {ray.origin[0] + retval.shape_dist * ray.direction[0], ray.origin[1] + retval.shape_dist * ray.direction[1], ray.origin[2] + retval.shape_dist * ray.direction[2]};

        // Find barycentric coords to determine of intersection_pt is inside triangle

        double area = magnitude_vec(face.norm_flat_shade);
        Vec edge_p = subtract_vec(retval.intersect_pt, face.p0);

        // Dot products
        double dot_11 = dot_product(edge1, edge1);
        double dot_12 = dot_product(edge1, edge2);
        double dot_22 = dot_product(edge2, edge2);
        double dot_1p = dot_product(edge1, edge_p);
        double dot_2p = dot_product(edge2, edge_p);

        double determinate = dot_11 * dot_22 - dot_12 * dot_12;
        if (determinate == 0) {
            // No solution
            continue;
        }

        double beta = (dot_22 * dot_1p - dot_12 * dot_2p) / determinate;
        double gamma = (dot_11 * dot_2p - dot_12 * dot_1p) / determinate;
        double alpha = 1 - (beta + gamma);

        if (alpha < 0 || beta < 0 || gamma < 0 || alpha > 1 || beta > 1 || gamma > 1) {
            continue;
        }
        if (std::abs((alpha + beta + gamma) - 1.0) >= 0.1) {
            continue;
        }

        // Sub triangles found with barycentric coords
        double sub_tri_a = alpha * area;
        double sub_tri_b = beta * area;
        double sub_tri_c = gamma * area;

        if (sub_tri_a < 0 || sub_tri_b < 0 || sub_tri_c < 0) {
            // intersection_pt is outside triangle
            continue;
        }

        // Calculate the normal vector at a point on a triangle face
        // Check if face was given normal vertex vectors from the file
        if (!face.hasNorm) {
            retval.normal_vec = face.norm_flat_shade;
        }
        else {
            // n = (a*n0 +b*n1 +g*n2) / ||a*n0 +b*n1 +g*n2||
            Vec temp = add_vec(add_vec(scale_vec(alpha, face.n0), scale_vec(beta, face.n1)), scale_vec(gamma, face.n2));
            retval.normal_vec = normalize_vec(scale_vec(1 / magnitude_vec(temp), temp));
        }

        retval.normal_vec = normalize_vec(retval.normal_vec);
        return retval;
    }

    // If outside for loop, no faces had an intersection
    return {};
}

MaterialColor TriangleMesh::tex_color(const HitResult& hit_result) {
    return base_color;
}