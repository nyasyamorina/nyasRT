#include <iostream>
#include <string>

#include "nyasRT.hpp"


template<class FP> std::string fp_name() noexcept;
template<> std::string fp_name<f32>() noexcept
{
    return "32-bit floating-point number";
}
template<> std::string fp_name<f64>() noexcept
{
    return "64-bit floating-point number";
}


#ifdef NDEBUG
constexpr vec2<u32> figure_size  = vec2(1280, 720);
constexpr u32 rays_per_pixel = 20;
constexpr u32 max_ray_bounds = 10;
#else
constexpr vec2<u32> figure_size  = vec2(640, 360);
constexpr u32 rays_per_pixel = 2;
constexpr u32 max_ray_bounds = 3;
#endif


using namespace std::chrono;
i32 main(i32, char * *)
{
    std::cout << " -- using " << fp_name<fg>() << " to calculate geometry operations" << std::endl;
    std::cout << " -- render config: figure size " << figure_size.x << 'x' << figure_size.y << ", "
    << rays_per_pixel << " rays/pixel, ray depth " << max_ray_bounds << std::endl;


    Figure fig(figure_size);

    Scence scence;

    MeshPtr mesh_p = Mesh::load_obj("../../models/teapot.obj");
    scence.objects.emplace_back(mesh_p);
    scence.objects.back().transform.rotate(defaults<vec3g>::Z, deg2rad(135.0)).scale(0.8).shift(vec3g(-2, -0.5, 0));
    scence.objects.emplace_back(mesh_p);
    scence.objects.back().transform.rotate(defaults<vec3g>::X, deg2rad(-90.0)).rotate(defaults<vec3g>::Z, deg2rad(-45.0)).scale(0.8).shift(vec3g(0, -4.3, 1.3));

    auto sky_p = std::make_shared<GradientSky>(RGB(0.25f, 0.5f, 1.0f), RGB(1.0f));
    scence.set_sky(sky_p);

    auto camera_p = std::make_shared<PerspectiveCamera>();
    camera_p->set_view_origin(vec3g(9, 10, 9.5)).set_view_direction(-vec3g(8.5, 10, 7.3)).set_aspect_ratio(fig.aspect_ratio()).set_field_of_view(deg2rad(45.0));
    scence.set_camera(camera_p);

    auto start_0 = system_clock::now();
    if (!scence.prepare())
    {
        std::cout << "scence prepare falid" << std::endl;
        return 1;
    }
    auto total_time_0 = system_clock::now() - start_0;
    auto total_sec_0 = static_cast<f32>(duration_cast<microseconds>(total_time_0).count()) / 1000000.0f;
    std::cout << "preparing time: " << total_sec_0 << "s" << std::endl;

    Renderer renderer(scence, rays_per_pixel, max_ray_bounds);

    /*auto start_1 = system_clock::now();
    renderer.render(fig);
    auto total_time_1 = system_clock::now() - start_1;
    auto total_sec_1 = static_cast<f32>(duration_cast<microseconds>(total_time_1).count()) / 1000000.0f;
    std::cout << "single thread rendering time: " << total_sec_1 << "s" << std::endl;
    fig.save<QOI>("../../out_1.qoi");*/

    auto start_16 = system_clock::now();
    renderer.render(fig, 16);
    auto total_time_16 = system_clock::now() - start_16;
    auto total_sec_16 = static_cast<f32>(duration_cast<microseconds>(total_time_16).count()) / 1000000.0f;
    std::cout << "16-thread rendering time: " << total_sec_16 << "s" << std::endl;

    for (RGB & pixel : fig) { pixel.apply_gamma(); }
    fig.save<QOI>("../../out_16.qoi");
    //fig.save<QOI>("../../out_trace_info.qoi");
}
