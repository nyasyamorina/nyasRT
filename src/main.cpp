#include <functional>
#include <iostream>
#include <string>

//#define SHOW_TRACE_INFO

#include "nyasRT.hpp"
#include "scences.hpp"


inline void timeit(std::string const& name, std::function<void()> const& func) noexcept
{
    using namespace std::chrono;

    auto time_start = system_clock::now();
    func();
    auto time_stop = system_clock::now();

    f32 milis = duration_cast<milliseconds>(time_stop - time_start).count();
    std::cout << "timing of \"" << name << "\": " << (milis / 1000) << 's' << std::endl;
}


#ifdef NDEBUG
constexpr vec2<u32> figure_size  = vec2(1280, 720);
constexpr u32 rays_per_pixel = 4096;
constexpr u32 max_ray_bounds = 32;
#else
constexpr vec2<u32> figure_size  = vec2(640, 360);
constexpr u32 rays_per_pixel = 2;
constexpr u32 max_ray_bounds = 3;
#endif


i32 main(i32, char * *)
{
    std::cout << " -- using " << ((sizeof(fg) == 4) ? "32" : "64") << "-bit floating-point number to calculate geometry operations" << std::endl;
    std::cout << " -- render config: figure size " << figure_size.x << 'x' << figure_size.y << ", "
    << rays_per_pixel << " rays/pixel, ray depth " << max_ray_bounds << std::endl;

    ScencePtr scence_p(nullptr);
    bool scence_prepared = false;
    Figure fig(figure_size);

    timeit("building scenece & figure", [&] ()
    {
        scence_p = two_torus_interlocking(0.3, 100, 200);
        //scence_p = two_teopot_interlocking();

        auto sky_p = std::make_shared<ArHosek>();
        sky_p->solar_direcion(vec3g(1, -0.5, 1.5)).albedo(RGB(0.9)).turbidity(4.5);
        //auto sky_p = std::make_shared<GradientSky>(RGB(0.4, 0.6, 0.8), RGB(1));

        scence_p->sky(sky_p);

        //auto texture0_p = std::make_shared<PureColor>(RGB(0.95).remove_gamma());
        //auto texture1_p = std::make_shared<PureColor>(RGB(0.15, 0.2, 0.6).remove_gamma());
        auto texture0_p = std::make_shared<PureColor>(RGB(0.9).remove_gamma());
        auto texture1_p = std::make_shared<FunctionalTexture>(RGB(0.97, 0.62, 0.31).remove_gamma(), RGB(0.53, 0.26, 0.07).remove_gamma());

        scence_p->objects[0].texture_p = texture0_p;
        scence_p->objects[1].texture_p = texture1_p;

        //auto brdf0_p = std::make_shared<DisneyBRDF>();
        //brdf0_p->roughness(0.1).metalic(0.9).subsurface(0.1).specular(0.9).clearcoat(0.05).sheen(0);
        //auto brdf1_p = std::make_shared<DisneyBRDF>();
        //brdf1_p->remove_gamma()).roughness(0.3).metalic(0.3).specular(0.3).clearcoat(0.9).sheen(0);
        auto brdf0_p = std::make_shared<SimpleSpecular>(); brdf0_p->roughness(0.02).clearcoat(0);
        auto brdf1_p = std::make_shared<SimpleSpecular>(); brdf1_p->roughness(0.8).clearcoat(0.2);

        scence_p->objects[0].brdf_p = brdf0_p;
        scence_p->objects[1].brdf_p = brdf1_p;

        scence_prepared = scence_p->prepare();
    });

    if (!scence_prepared)
    {
        std::cout << "scence prepare falid" << std::endl;
        return 1;
    }

    Renderer renderer(*scence_p, rays_per_pixel, max_ray_bounds);

    timeit("rendering using 16 threads", [&] ()
    {
        renderer.render(fig, 16);
    });

#ifdef SHOW_TRACE_INFO
    fig.save<QOI>("../../out_trace_info.qoi");
#else

    timeit("post-processing", [&] ()
    {
        output_ready(ACES_tone_mapping(auto_exposure(fig)));
    });

    fig.save<QOI>("../../out_16.qoi");
#endif
}
