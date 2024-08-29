// store some useful scences without BRDFs

#pragma once

#include "nyasRT.hpp"


ScencePtr two_teopot_interlocking()
{
    ScencePtr scence_p = std::make_shared<Scence>();
    auto & objs = scence_p->objects;

    MeshPtr mesh_p = Mesh::load_obj("../../models/teapot.obj");
    mesh_p->enable_normal_interpolation = true;

    objs.emplace_back(); objs.back().mesh_p = mesh_p;
    objs.back().transform.rotate(defaults<vec3g>::Z, deg2rad(135.0)).scale(0.8).shift(vec3g(-2, -0.5, 0));

    objs.emplace_back(); objs.back().mesh_p = mesh_p;
    objs.back().transform.rotate(defaults<vec3g>::X, deg2rad(-90.0)).rotate(defaults<vec3g>::Z, deg2rad(-45.0)).scale(0.8).shift(vec3g(0, -4.3, 1.3));

    auto camera_p = std::make_shared<PerspectiveCamera>();
    camera_p->view_origin(vec3g(9, 10, 9.5)).view_direction(-vec3g(8.5, 10, 7.3)).aspect_ratio(16.0f/9.0f).field_of_view(deg2rad(40.0));
    scence_p->camera(camera_p);

    auto sky_p = std::make_shared<GradientSky>(RGB(0.5f, 0.7f, 1.0f).remove_gamma(), RGB(1.0f));
    scence_p->sky(sky_p);

    return scence_p;
}

ScencePtr two_torus_interlocking(fg tube_radius, u32 n_a, u32 n_b)
{
    auto scence_p = std::make_shared<Scence>();
    auto & objs = scence_p->objects;

    auto mesh_p = Mesh::torus(tube_radius, n_a, n_b);
    mesh_p->enable_normal_interpolation = true;
    mesh_p->custom_vertex_normals = false;

    objs.emplace_back(); objs.back().mesh_p = mesh_p;
    objs.back().transform.rotate(defaults<vec3g>::X, deg2rad(45.0)).shift(vec3g(-0.5, 0, 0));

    objs.emplace_back(); objs.back().mesh_p = mesh_p;
    objs.back().transform.rotate(defaults<vec3g>::X, deg2rad(-45.0)).shift(vec3g(0.5, 0, 0));

    fg floor_z = -tube_radius - 0.707;
    auto floor_p = std::make_shared<Mesh>();
    floor_p->vertices.emplace_back(   0, -500, floor_z);
    floor_p->vertices.emplace_back( 500,  500, floor_z);
    floor_p->vertices.emplace_back(-500,  500, floor_z);
    floor_p->faces.emplace_back(0, 1, 2);
    auto floor_texture_p = std::make_shared<PureColor>(RGB(0.6, 0.75, 0.85));
    auto floor_brdf_p = std::make_shared<SimpleSpecular>();
    floor_brdf_p->clearcoat(0.6).roughness(0.4);
    objs.emplace_back(floor_p, floor_texture_p, floor_brdf_p);

    vec3g view_point = vec3g(20, -100, 20);
    auto camera_p = std::make_shared<PerspectiveCamera>();
    camera_p->view_origin(view_point).view_direction(-view_point).aspect_ratio(16.0f/9.0f).field_of_view(deg2rad(3.5));
    scence_p->camera(camera_p);

    auto sky_p = std::make_shared<GradientSky>(RGB(0.5f, 0.7f, 1.0f).remove_gamma(), RGB(1.0f));
    scence_p->sky(sky_p);

    return scence_p;
}
