#include <iostream>
#include <string>

#include "utils.hpp"
#include "geometry/vec2.hpp"
#include "geometry/vec3.hpp"
#include "geometry/vec4.hpp"
#include "RGB.hpp"
#include "geometry/Ray.hpp"
#include "geometry/BoundingBox.hpp"
#include "geometry/Mesh.hpp"
#include "Object3D.hpp"
#include "cameras/Camera.hpp"
#include "cameras/PerspectiveCamera.hpp"
#include "Figure.hpp"
#include "random.hpp"
#include "Scence.hpp"
#include "Renderer.hpp"


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
constexpr u32 figure_height = 720;
constexpr u32 figure_width  = 1280;
constexpr u32 rays_per_pixel = 20;
constexpr u32 ray_depth = 10;
#else
constexpr u32 figure_height = 360;
constexpr u32 figure_width  = 640;
constexpr u32 rays_per_pixel = 2;
constexpr u32 ray_depth = 3;
#endif


i32 main(i32, char * *)
{
    std::cout << " -- using " << fp_name<fg>() << " to calculate geometry operations" << std::endl;
    std::cout << " -- render config: figure size " << figure_width << 'x' << figure_height << ", "
    << rays_per_pixel << " rays/pixel, ray depth " << ray_depth << std::endl;


    Figure fig(figure_height, figure_width);


    Scence scence;

    MeshPtr mesh_p = Mesh::load_obj("../../models/teapot.obj");
    scence.objects.emplace_back(mesh_p);
    scence.objects.back().transform.rotate(defaults<vec3g>::Z, deg2rad(135.0)).scale(0.8).offset(vec3g(-2, -0.5, 0));
    scence.objects.emplace_back(mesh_p);
    scence.objects.back().transform.rotate(defaults<vec3g>::X, deg2rad(-90.0)).rotate(defaults<vec3g>::Z, deg2rad(-45.0)).scale(0.8).offset(vec3g(0, -4.3, 1.3));

    auto camera_p = std::make_shared<PerspectiveCamera>();
    camera_p->set_view_origin(vec3g(8, 10, 9)).set_view_direction(-vec3g(8, 10, 7.5)).set_aspect_ratio(fig.aspect_ratio()).set_field_of_view(deg2rad(45.0));
    scence.set_camera(camera_p);

    if (!scence.prepare())
    {
        std::cout << "scence prepare falid" << std::endl;
        return 1;
    }

    Renderer renderer(scence, rays_per_pixel, ray_depth);

    using namespace std::chrono;

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
    fig.save<QOI>("../../out_16.qoi");
}