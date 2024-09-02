// store some useful scences without BRDFs

#pragma once

#include "nyasRT.hpp"


ScencePtr two_teopot_interlocking()
{
    ScencePtr scence_p = std::make_shared<Scence>();
    auto & objs = scence_p->objects;

    MeshPtr mesh_p = Mesh::load_obj("../../models/teapot.obj");
    mesh_p->enable_normal_interpolation = true;
    mesh_p->custom_vertex_normals = false;

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

    objs.emplace_back(); objs.back().mesh_p = mesh_p;
    objs.back().transform.rotate(defaults<vec3g>::X, deg2rad(45.0)).shift(vec3g(-0.5, 0, 0));

    objs.emplace_back(); objs.back().mesh_p = mesh_p;
    objs.back().transform.rotate(defaults<vec3g>::X, deg2rad(-45.0)).shift(vec3g(0.5, 0, 0));

    fg floor_z = -tube_radius - 0.707;
    auto floor_p = std::make_shared<Mesh>();
    u32 floor_A = floor_p->add_vertex(vec3g(   0, -500, floor_z));
    u32 floor_B = floor_p->add_vertex(vec3g( 500,  500, floor_z));
    u32 floor_C = floor_p->add_vertex(vec3g(-500,  500, floor_z));
    floor_p->add_face(floor_A, floor_B, floor_C);
    auto floor_material_p = std::make_shared<PureColor>(RGB(0.6, 0.75, 0.85));
    //auto floor_brdf_p = std::make_shared<SimpleSpecular>();
    //floor_brdf_p->roughness(0.7).clearcoat(0.25);
    auto floor_brdf_p = std::make_shared<DisneyBRDF>();
    floor_brdf_p->subsurface(0.5).metalic(0.2).specular(0.3).roughness(0.8).sheen(0);
    objs.emplace_back(floor_p, floor_material_p, floor_brdf_p);

    vec3g view_point = vec3g(20, -100, 20);
    auto camera_p = std::make_shared<PerspectiveCamera>();
    camera_p->view_origin(view_point).view_direction(-view_point).aspect_ratio(16.0f/9.0f).field_of_view(deg2rad(3.5));
    scence_p->camera(camera_p);

    auto sky_p = std::make_shared<GradientSky>(RGB(0.5f, 0.7f, 1.0f).remove_gamma(), RGB(1.0f));
    scence_p->sky(sky_p);

    return scence_p;
}


Figure render_Hosek_sky_examples()
{
    constexpr u32 single_size = 200;
    constexpr u32 n_elevation = 23, n_turbidity = 10;
    constexpr RGB albedo = RGB(1);

    Hosek sky; sky.albedo(albedo);
    Figure fig(vec2(single_size * n_elevation, single_size * n_turbidity));

    for (u32 ttt = 0; ttt < n_turbidity;  ttt++)
    {
        for (u32 eee = 0; eee < n_elevation; eee++)
        {
            fg solar_elevation = defaults<fg>::half_pi * eee / (n_elevation - 1);
            f32 turbidity = 9.0f * ttt / (n_turbidity - 1) + 1;

            sky.turbidity(turbidity).solar_direcion(vec3g(0, -std::cos(solar_elevation), std::sin(solar_elevation)));
            sky.prepare();

            vec2<u32> tl(eee * single_size, ttt * single_size);
            ViewedFigure view(fig, tl, tl + single_size);

            auto stop = view.end();
            for (auto pixel = view.begin(); pixel < stop; pixel++)
            {
                vec2g position = (pixel.index - tl) * 2.0f / single_size - 1;
                fg r = length(position);

                if (r > 1)
                {
                    *pixel = defaults<RGB>::Black;
                    continue;
                }
                fg z = std::sqrt(1 - sqr(r));
                *pixel = sky(vec3g(position, z));
            }
        }
    }

    output_ready(/*ACES_tone_mapping*/(auto_exposure(fig)));
    return fig;
}
