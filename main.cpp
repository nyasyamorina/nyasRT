#include <iostream>
#include <memory>

#define GLM_FORCE_AVX2

//#define NYASRT_USE_DOUBLE_PRECISION_GEOMETRY
//#define NYASRT_SHOW_TRACE_INFO
//#define NYASRT_DISPLAY_PROGRESS

#include "src/nyasRT.hpp"
#include "scences.hpp"
using namespace nyasRT::basic_types;
using namespace nyasRT::image_formats;


#ifdef NDEBUG
size2_t const figure_size  = size2_t(1280, 720);
constexpr u32 rays_per_pixel = 4096;
constexpr u32 max_ray_bounds = 32;
#else
size2_t const figure_size  = size2_t(640, 360);
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
    nyasRT::GraphicsBuffer fig(figure_size);

    f32 building_time = nyasRT::timeit([&] ()
    {
        using nyasRT::remove_gamma;

        scence_p = two_torus_interlocking(0.3, 30, 90);
        //scence_p = two_teopot_interlocking();

        auto sky_p = std::make_shared<nyasRT::sky_models::Hosek>();
        sky_p->solar_direcion(vec3g(1, -0.5, 1.5)).albedo(RGB(0.9)).turbidity(4.5);

        scence_p->sky(sky_p);
        scence_p->light_ps.push_back(sky_p->sun());

        auto material0_p = std::make_shared<nyasRT::materials::PureColor>(remove_gamma(RGB(0.76, 0.76, 0.86)));
        //auto material1_p = std::make_shared<nyasRT::materials::FunctionalMaterial>(remove_gamma(RGB(0.97, 0.62, 0.31)), remove_gamma(RGB(0.53, 0.26, 0.07)));
        auto material1_p = nyasRT::materials::Texture::load<QOI>(nyasRT::Interpolation::Bicubic, "../../.textures/donut.qoi");

        scence_p->objects[0].material_p = material0_p;
        scence_p->objects[1].material_p = material1_p;

        auto brdf0_p = std::make_shared<nyasRT::BRDFs::DisneyBRDF>();
        brdf0_p->subsurface(0.2).metalic(1.0).specular(0.9).specular_tint(1.0).roughness(0.2).sheen(0).clearcoat(0.0).clearcoat_gloss(0.9);
        auto brdf1_p = std::make_shared<nyasRT::BRDFs::DisneyBRDF>();
        brdf1_p->subsurface(0.5).metalic(0.0).specular(0.5).specular_tint(0.0).roughness(0.8).sheen(0).clearcoat(9.9).clearcoat_gloss(0.8);
        //auto brdf0_p = std::make_shared<nyasRT::BRDFs::SimplyWrongSpecular>(); brdf0_p->roughness(0.03).clearcoat(0.01);
        //auto brdf1_p = std::make_shared<nyasRT::BRDFs::SimplyWrongSpecular>(); brdf1_p->roughness(0.55).clearcoat(0.05);

        scence_p->objects[0].brdf_p = brdf0_p;
        scence_p->objects[1].brdf_p = brdf1_p;

        scence_prepared = scence_p->prepare();
    });

    if (!scence_prepared)
    {
        std::cout << "scence prepare falid" << std::endl;
        return 1;
    }
    std::cout << "timing of \"" << "building scenece" << "\": " << building_time << 's' << std::endl;

    nyasRT::RenderConfig config{nyasRT::SampleType::MultiJittered, 83u, rays_per_pixel, max_ray_bounds};
    nyasRT::Renderer renderer(*scence_p, config);

    f32 rendering_time = nyasRT::timeit([&] ()
    {
        renderer.render(fig, 16);
    });
    std::cout << "timing of \"" << "rendering" << "\": " << rendering_time << 's' << std::endl;

#ifdef NYASRT_SHOW_TRACE_INFO
    if (!(QOI().save("../../out_trace_info.qoi", fig)))
#else

    output_ready(ACES_tone_mapping(auto_exposure(fig)));

    if (!(QOI().save("../../out.qoi", fig)))
#endif
    {
        std::cout << "failed to write figure into file" << std::endl;
    }

    //QOI().save("../../out_Hosek.qoi", render_Hosek_sky_examples());
}
