#include <iostream>
#include <stdint.h>
#include <memory>
#include <type_traits>

#include "setup.h"
#include "nyasRT.h"

using namespace std;
using namespace sample_types;
using namespace generators;
using namespace materials;
using namespace objects;
using namespace cameras;
using namespace ray_tracers;


Worldp scenes1(uint64_t fig_h, uint64_t fig_w, uint64_t samples, uint32_t depth);


int main()
{
#ifdef LOAD_SHOW_SAMPLE_FUNCTIONS
    using namespace sample_types;
    using namespace generators;

    uint64_t n_sets = 83, n_samples = 256;
    Generatorp gen = make_shared<CorrectMJ>();

    show_samples(Sampler(Square(), n_sets, n_samples, gen)).save_to(R"(outputs\show_samples\square.png)");
    show_samples(Sampler(Circle(), n_sets, n_samples, gen)).save_to(R"(outputs\show_samples\circle.png)");
    show_samples(Sampler(Sphere(), n_sets, n_samples, gen)).save_to(R"(outputs\show_samples\sphere.png)");
    show_samples(Sampler(Hemisphere(10), n_sets, n_samples, gen)).save_to(R"(outputs\show_samples\hemisphere.png)");

    show_gen_perform(n_sets, 64, make_shared<Uniform>()).save_to(R"(outputs\show_gen_perform\uniform.png)");
    show_gen_perform(n_sets, 64, make_shared<Hammersley>()).save_to(R"(outputs\show_gen_perform\hammersley.png)");
    show_gen_perform(n_sets, 64, make_shared<Halton>()).save_to(R"(outputs\show_gen_perform\halton.png)");
    show_gen_perform(n_sets, 64, make_shared<Random>()).save_to(R"(outputs\show_gen_perform\random.png)");
    show_gen_perform(n_sets, 64, make_shared<Jittered>()).save_to(R"(outputs\show_gen_perform\jittered.png)");
    show_gen_perform(n_sets, 64, make_shared<NRooks>()).save_to(R"(outputs\show_gen_perform\nRooks.png)");
    show_gen_perform(n_sets, 64, make_shared<MultiJ>()).save_to(R"(outputs\show_gen_perform\multij.png)");
    show_gen_perform(n_sets, 64, make_shared<CorrectMJ>()).save_to(R"(outputs\show_gen_perform\correctmj.png)");
#endif // LOAD_SHOW_SAMPLE_FUNCTIONS

    auto world_p = scenes1(480, 853, 64, 10);
    render_figure(*world_p);
    world_p->camera_p->figure.save_to(R"(outputs/scenes1.png)");

    return 0;
}



void render_figure(World & world) {
    auto height = world.camera_p->figure.get_height();
    auto width = world.camera_p->figure.get_width();
    auto samples = world.camera_p->sampler_p->samples_per_set();

    for (uint64_t h = 0; h < height; ++h) {
        for (uint64_t w = 0; w < width; ++w) {
            auto pixel_color = RGB(0.f);
            for (uint64_t s = 0; s < samples; ++s) {
                auto ray = world.camera_p->get_ray(h, w);
                auto ray_color = world.tracer_p->trace_from_camera(ray,
                            world.ray_depth);
                pixel_color += ray_color;
            }
            world.camera_p->figure(h, w) = pixel_color / (float)samples;
        }
    }
}


Worldp scenes1(uint64_t fig_h, uint64_t fig_w, uint64_t samples, uint32_t depth) {
    auto world = make_shared<World>(depth);
    world->tracer_p = make_shared<RT1>(*world);

    auto cam_s = make_sampler(Square(), 83, samples);
    auto cam_p = Vec3(0, 10, 7);
    auto look_at = Vec3(0, 0, 1);
    world->camera_p = make_shared<Pinhole>(fig_h, fig_w, cam_s, cam_p,
                                           look_at - cam_p, deg2rad(70));

    auto ball_mat_s = make_sampler(Hemisphere(1), 83, samples);
    auto ball_mat = make_shared<Matte>(RGB(.9f, .5f, .5f), ball_mat_s);
    auto ball = make_shared<objects::Sphere>(ball_mat, Vec3(0, 0, 2), 2);
    world->object_ps.push_back(ball);

    auto floor_mat_s = make_sampler(Hemisphere(1), 83, samples);
    auto floor_mat = make_shared<Matte>(RGB(.95f), floor_mat_s);
    auto floor = make_shared<objects::Sphere>(floor_mat, Vec3(0, 0, -100), 100);
    world->object_ps.push_back(floor);

    return world;
}
