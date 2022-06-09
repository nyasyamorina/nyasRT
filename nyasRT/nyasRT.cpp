#include <iostream>
#include <stdint.h>
#include <memory>
#include <type_traits>
#include <chrono>

#include "setup.h"
#include "nyasRT.h"

using namespace std;
using namespace chrono;
using namespace remap_colors;
using namespace sample_types;
using namespace generators;
using namespace lights;
using namespace brdfs;
using namespace materials;
using namespace objects;
using namespace cameras;
using namespace ray_tracers;


Worldp scenes1(uint64_t fig_h, uint64_t fig_w, uint64_t samples, uint32_t depth);
Worldp scenes2(uint64_t fig_h, uint64_t fig_w, uint64_t samples, uint32_t depth);
Worldp scenes3(uint64_t fig_h, uint64_t fig_w, uint64_t samples, uint32_t depth);
Worldp scenes4(uint64_t fig_h, uint64_t fig_w, uint64_t samples, uint32_t depth);
Worldp scenes5(uint64_t fig_h, uint64_t fig_w, uint64_t samples, uint32_t depth);

#define TEST_CAMERA_TYPE 3      // 1: Pinhole, 2: Parallel, 3: Lens, 4: Fisheye, 5: Spherical
Camerap for_testing(uint64_t fig_h, uint64_t fig_w, uint64_t samples);
Worldp test_camera(Camerap cam, uint64_t samples, uint32_t depth);


int main()
{
#ifdef LOAD_SHOW_SAMPLE_FUNCTIONS
    using namespace sample_types;
    using namespace generators;

    uint64_t n_sets = 83, n_samples = 256;
    Generatorp gen = make_shared<CorrectMJ>();

    show_samples(Sampler(Square(), n_sets, n_samples, gen)).save_to(R"(outputs\show_samples\square.png)");
    show_samples(Sampler(sample_types::Circle(), n_sets, n_samples, gen)).save_to(R"(outputs\show_samples\circle.png)");
    show_samples(Sampler(sample_types::Sphere(), n_sets, n_samples, gen)).save_to(R"(outputs\show_samples\sphere.png)");
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

    uint64_t const samples = 256;
    //auto world_p = scenes5(480, 853, samples, 10);
    auto world_p = test_camera(for_testing(480, 960, samples), samples, 10);

    auto start = system_clock::now();
            render_figure(*world_p, Clamp01());
    auto dura = duration_cast<milliseconds>(system_clock::now() - start);
    cout << "rendering time: " << dura.count() / 1000.f << "s" << endl;

    world_p->camera_p->figure.save_to(R"(outputs/scenes5.png)");

    return 0;
}



void render_figure(World & world, RemapColor const& remap) {
    auto height = world.camera_p->figure.get_height();
    auto width = world.camera_p->figure.get_width();
    auto samples = world.camera_p->sampler_p->samples_per_set();

    //// show the point light
    //uint32_t total_show = 0;
    //vector<objects::Sphere> pl_sph;
    //vector<RGB> pl_color;
    //for (auto & light_p : world.light_ps) {
    //    auto point_p = dynamic_cast<Point *>(light_p.get());
    //    if (point_p != nullptr) {
    //        pl_sph.push_back(objects::Sphere(nullptr, point_p->point, 0.1));
    //        pl_color.push_back(point_p->color);
    //        total_show += 1;
    //    }
    //}

    for (uint64_t h = 0; h < height; ++h) {
        for (uint64_t w = 0; w < width; ++w) {
            auto pixel_color = RGB(0.f);
            for (uint64_t s = 0; s < samples; ++s) {
                auto ray = world.camera_p->get_ray(h, w);
                auto ray_color = world.tracer_p->trace_from_camera(ray, world.ray_depth);

                //// show the point light
                //for (uint32_t i = 0; i < total_show; ++i) {
                //    if (pl_sph[i].hit(ray, inf)) {
                //        ray_color = pl_color[i];
                //    }
                //}

                pixel_color += ray_color;
            }
            world.camera_p->figure(h, w) = remap(pixel_color / (float)samples);
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


Worldp scenes2(uint64_t fig_h, uint64_t fig_w, uint64_t samples, uint32_t depth) {
    auto world = make_shared<World>(depth);
    world->tracer_p = make_shared<RayTracer>(*world);

    auto cam_s = make_sampler(Square(), 83, samples);
    auto cam_p = Vec3(0, 10, 7);
    auto look_at = Vec3(0, 0, 1);
    world->camera_p = make_shared<Pinhole>(fig_h, fig_w, cam_s, cam_p,
                                           look_at - cam_p, deg2rad(70));

    auto ball_brdf = make_shared<Lambertian>(83, samples);
    auto ball_mat = make_shared<Opaque>(ball_brdf, RGB(.9f, .5f, .5f));
    auto ball = make_shared<objects::Sphere>(ball_mat, Vec3(0, 0, 2), 2);
    world->object_ps.push_back(ball);

    auto floor_brdf = make_shared<Lambertian>(83, samples);
    auto floor_mat = make_shared<Opaque>(floor_brdf, RGB(.95f));
    auto floor = make_shared<objects::Sphere>(floor_mat, Vec3(0, 0, -100), 100);
    world->object_ps.push_back(floor);

    auto light1 = make_shared<lights::Parallel>(RGB(1.f), Vec3(-1.));
    world->light_ps.push_back(light1);

    auto light2 = make_shared<Point>(75.f * RGB(.6f, .8f, 1.f), Vec3(-5., 0., 4.5));
    world->light_ps.push_back(light2);

    return world;
}


Worldp scenes3(uint64_t fig_h, uint64_t fig_w, uint64_t samples, uint32_t depth) {
    auto world = make_shared<World>(depth);
    world->tracer_p = make_shared<RayTracer>(*world);

    auto cam_s = make_sampler(Square(), 83, samples);
    auto cam_p = Vec3(0, 10, 7);
    auto look_at = Vec3(0, 0, 1);
    world->camera_p = make_shared<Pinhole>(fig_h, fig_w, cam_s, cam_p,
                                           look_at - cam_p, deg2rad(70));

    auto ball_brdf = make_shared<Lambertian>(83, samples);
    auto ball_mat = make_shared<Opaque>(ball_brdf, RGB(.9f, .5f, .5f));
    auto ball = make_shared<objects::Sphere>(ball_mat, Vec3(0, 0, 2), 2);
    world->object_ps.push_back(ball);

    auto floor_brdf = make_shared<Lambertian>(83, samples);
    auto floor_mat = make_shared<Opaque>(floor_brdf, RGB(.95f));
    auto floor = make_shared<objects::Sphere>(floor_mat, Vec3(0, 0, -100), 100);
    world->object_ps.push_back(floor);

    auto light_mat = make_shared<Emissive>(200.f * RGB(1.f, .8f, .6f));
    auto light = make_shared<objects::Sphere>(light_mat, Vec3(-5., -1., 4.5), .5f);
    light->sampler_p = make_sampler(sample_types::Sphere(), 83, samples);
    world->object_ps.push_back(light);
    world->light_ps.push_back(make_shared<ObjectLight>(light, *world));

    return world;
}


Worldp scenes4(uint64_t fig_h, uint64_t fig_w, uint64_t samples, uint32_t depth) {
    auto world = make_shared<World>(depth);
    world->tracer_p = make_shared<RayTracer>(*world);

    auto cam_s = make_sampler(Square(), 83, samples);
    world->camera_p = make_shared<Pinhole>(fig_h, fig_w, cam_s, Vec3(0, -6, 0),
                                           Vec3(0, 6, 0), deg2rad(90));

    auto circ_brdf = make_shared<Lambertian>(83, samples);
    auto circ_mat = make_shared<Opaque>(circ_brdf, RGB(.95f));
    auto circ1 = make_shared<objects::Circle>(circ_mat, Vec3(-2, 0, 0), Vec3(1, 0, 0), 2);
    auto circ2 = make_shared<objects::Circle>(circ_mat, Vec3(2, 0, 0), Vec3(1, 0, 0), 2);
    world->object_ps.push_back(circ1);
    world->object_ps.push_back(circ2);

    auto light1_mat = make_shared<Emissive>(200.f * RGB(1.f, .8f, .6f));
    auto light1 = make_shared<objects::Sphere>(light1_mat, Vec3(0), .1f);
    light1->sampler_p = make_sampler(sample_types::Sphere(), 83, samples);
    world->object_ps.push_back(light1);
    world->light_ps.push_back(make_shared<ObjectLight>(light1, *world));

    auto light2_mat = make_shared<Emissive>(200.f * RGB(.6f, .8f, 1.f));
    auto light2 = make_shared<objects::Sphere>(light2_mat, Vec3(4, 0, 0), .1f);
    light2->sampler_p = make_sampler(sample_types::Sphere(), 83, samples);
    world->object_ps.push_back(light2);
    world->light_ps.push_back(make_shared<ObjectLight>(light2, *world));

    return world;
}


Worldp scenes5(uint64_t fig_h, uint64_t fig_w, uint64_t samples, uint32_t depth) {
    auto world = make_shared<World>(depth);
    world->tracer_p = make_shared<RayTracer>(*world);

    auto cam_s = make_sampler(Square(), 83, samples);
    world->camera_p = make_shared<Pinhole>(fig_h, fig_w, cam_s, Vec3(12, -12, 12),
                                           Vec3(-12, 12, -12), deg2rad(70));

    auto floor_brdf = make_shared<Lambertian>(83, samples);
    auto floor_mat = make_shared<Opaque>(floor_brdf, RGB(.95f));
    auto floor = make_shared<objects::Circle>(floor_mat, Vec3(0), Vec3(0, 0, 1), 100);
    world->object_ps.push_back(floor);

    auto light_mat = make_shared<Emissive>(100.f * RGB(1.f, .8f, .6f));
    auto light = make_shared<objects::Circle>(light_mat, Vec3(0, 0, 4), Vec3(1, 0, 0), 1);
    light->sampler_p = make_sampler(sample_types::Circle(), 83, samples);
    world->object_ps.push_back(light);
    world->light_ps.push_back(make_shared<ObjectLight>(light, *world));

    return world;
}



auto const viewp = Vec3(8, -16, 5);
auto const viewd = Vec3(-0.3, 1, -0.2);
#if (TEST_CAMERA_TYPE == 1)
Camerap for_testing(uint64_t fig_h, uint64_t fig_w, uint64_t samples) {
    auto fs = make_sampler(Square(), 83, samples);
    return make_shared<Pinhole>(fig_h, fig_w, fs, viewp, viewd, deg2rad(50));
}
#elif (TEST_CAMERA_TYPE == 2)
Camerap for_testing(uint64_t fig_h, uint64_t fig_w, uint64_t samples) {
    auto fs = make_sampler(Square(), 83, samples);
    return make_shared<cameras::Parallel>(fig_h, fig_w, fs, viewp, viewd, 17.5);
}
#elif (TEST_CAMERA_TYPE == 3)
auto const focus_dist = 18.13 + 3.6 * 1;
auto const lens_r = 0.5;
Camerap for_testing(uint64_t fig_h, uint64_t fig_w, uint64_t samples) {
    auto focold = viewd * (focus_dist / abs(viewd));
    auto fs = make_sampler(Square(), 83, samples);
    auto ls = make_sampler(sample_types::Circle(), 83, samples);
    return make_shared<Lens>(fig_h, fig_w, fs, viewp, focold, deg2rad(50), lens_r, ls);
}
#elif (TEST_CAMERA_TYPE == 4)
auto const fov = 200;   // deg
Camerap for_testing(uint64_t fig_h, uint64_t fig_w, uint64_t samples) {
    auto fs = make_sampler(Square(), 83, samples);
    return make_shared<Fisheye>(fig_h, fig_w, fs, viewp, viewd, deg2rad(fov));
}
#elif (TEST_CAMERA_TYPE == 5)
Camerap for_testing(uint64_t fig_h, uint64_t fig_w, uint64_t samples) {
    auto fs = make_sampler(Square(), 83, samples);
    return make_shared<Spherical>(fig_h, fig_w, fs, viewp, viewd);
}
#endif

Worldp test_camera(Camerap cam, uint64_t samples, uint32_t depth) {
    auto world = make_shared<World>(depth);
    world->tracer_p = make_shared<RT1>(*world);
    world->camera_p = cam;

    auto floor_brdf = make_shared<Lambertian>(83, samples);
    auto floor_mat = make_shared<Opaque>(floor_brdf, RGB(.95f));
    auto floor = make_shared<objects::Circle>(floor_mat, Vec3(0), Vec3(0, 0, 1), 100);
    world->object_ps.push_back(floor);

    auto ball1_brdf = make_shared<Lambertian>(83, samples);
    auto ball1_mat = make_shared<Opaque>(ball1_brdf, RGB(1.f, .5f, .3f));
    auto ball1 = make_shared<objects::Sphere>(ball1_mat, Vec3(0, 0, 2), 2);
    world->object_ps.push_back(ball1);

    auto ball2_brdf = make_shared<Lambertian>(83, samples);
    auto ball2_mat = make_shared<Opaque>(ball2_brdf, RGB(.3f, 1.f, .5f));
    auto ball2 = make_shared<objects::Sphere>(ball2_mat, Vec3(0, 4, 2), 2);
    world->object_ps.push_back(ball2);

    auto ball3_brdf = make_shared<Lambertian>(83, samples);
    auto ball3_mat = make_shared<Opaque>(ball3_brdf, RGB(.3f, .5f, 1.f));
    auto ball3 = make_shared<objects::Sphere>(ball3_mat, Vec3(0, 8, 2), 2);
    world->object_ps.push_back(ball3);

    auto ball4_brdf = make_shared<Lambertian>(83, samples);
    auto ball4_mat = make_shared<Opaque>(ball4_brdf, RGB(.2f, .6f, .4f));
    auto ball4 = make_shared<objects::Sphere>(ball4_mat, Vec3(0, -16, 2), 2);
    world->object_ps.push_back(ball4);

    auto ball5_brdf = make_shared<Lambertian>(83, samples);
    auto ball5_mat = make_shared<Opaque>(ball5_brdf, RGB(.7f));
    auto ball5 = make_shared<objects::Sphere>(ball5_mat, Vec3(16, -16, 2), 2);
    world->object_ps.push_back(ball5);

    return world;
}
