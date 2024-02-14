#include <stdio.h>
#include <math.h>
#include "view_window.h"

Window::Window(Options input) {
    // Get data from input file using Options 
    in_hfov = input.hfov;
    pic_width = static_cast<double>(input.width);
    pic_height = static_cast<double>(input.height);

    eye[0] = input.eye_pos[0];
    eye[1] = input.eye_pos[1];
    eye[2] = input.eye_pos[2];

    view_dir[0] = input.view_direc[0];
    view_dir[1] = input.view_direc[1];
    view_dir[2] = input.view_direc[2];

    up_dir[0] = input.up_direc[0];
    up_dir[1] = input.up_direc[1];
    up_dir[2] = input.up_direc[2];

    compute_width();
    compute_height();
    compute_u_and_v();
    compute_corners();
}

void Window::compute_width() {
    win_width = 2.0 * dist * tan(in_hfov * 0.5);
}

void Window::compute_height() {
    double aspect_ratio = pic_width / pic_height;
    win_height = win_width / aspect_ratio;
}

void Window::compute_u_and_v() {
    u = normalize_vec(cross_product(view_dir, up_dir));
    v = normalize_vec(cross_product(u, view_dir));
    // u and v are normalized
}

void Window::compute_corners() { 
    // Corners of window
    // ul = view_origin + d*n – (w/2)*u + (h/2)*v
    // ur = view_origin + d*n + (w/2)*u + (h/2)*v
    // ll = view_origin + d*n – (w/2)*u – (h/2)*v
    // lr = view_origin + d*n + (w/2)*u – (h/2)*v
    view_dir = normalize_vec(view_dir);

    Point dn_term = scale_vec(dist, view_dir);
    Point wu_term = scale_vec((win_width / 2.0), u);
    Point hv_term = scale_vec((win_height / 2.0), v);

    ul = add_vec(subtract_vec(add_vec(eye, dn_term), wu_term), hv_term); 
    ur = add_vec(add_vec(add_vec(eye, dn_term), wu_term), hv_term); 
    ll = subtract_vec(subtract_vec(add_vec(eye, dn_term), wu_term), hv_term); 
    lr = subtract_vec(add_vec(add_vec(eye, dn_term), wu_term), hv_term); 
}