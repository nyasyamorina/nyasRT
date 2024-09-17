#pragma once

#include <algorithm>
#include <math.h>
#include <mutex>
#include <thread>
#include <vector>

#include "common.hpp"
#include "geometry/Ray.hpp"
#include "graphics/GraphicsBuffer.hpp"
#include "graphics/DisplayWindow.hpp"
#include "components/Object3D.hpp"
#include "Sampler.hpp"
#include "Scence.hpp"


namespace nyasRT
{
class RenderConfig
{
public:

    SampleType sample_type;
    u32 n_sample_sets;
    u32 rays_pre_pixel, max_ray_bounds;
};


class Renderer
{
protected:

#if (GLM_HAS_CONSTEXPR)
    static VEC_CONSTEXPR index2_t task_size = index2_t(16, 16);
#else
    static index2_t const task_size;
#endif

    class TasksManager final
    {
    private:

        index2_t * tasks;
        index2_t * next_task;
        index2_t * tasks_stop;

    public:

        TasksManager(GraphicsBuffer const& gbuf)
        : tasks{nullptr}, next_task{nullptr}, tasks_stop{nullptr} {
            index2_t const gsize = gbuf.size();
            index2_t const center = gsize >> 1;
            index2_t start_start = center % task_size;
            if (start_start.x > 0) { start_start.x -= task_size.x; }
            if (start_start.y > 0) { start_start.y -= task_size.y; }

            index2_t const tmp = (gsize - 1 - start_start) / task_size + 1;
            u32 total_tasks = tmp.x * tmp.y;
            tasks = next_task = new index2_t[total_tasks];
            tasks_stop = tasks + total_tasks;

            for (i64 start_y = start_start.y; start_y < gbuf.height(); start_y += task_size.y)
            {
                for (i64 start_x = start_start.x; start_x < gbuf.width(); start_x += task_size.x)
                {
                    *(next_task++) = index2_t(start_x, start_y);
                }
            }
            next_task = tasks;

            std::sort(tasks, tasks_stop, [center] (index2_t l, index2_t r) noexcept -> bool
            {
                l += task_size / 2; r += task_size / 2;
                i32 ld = (l.x < center.x) ? (center.x - l.x) : (l.x - center.x);
                ld += (l.y < center.y) ? (center.y - l.y) : (l.y - center.y);
                i32 rd = (r.x < center.x) ? (center.x - r.x) : (r.x - center.x);
                rd += (r.y < center.y) ? (center.y - r.y) : (r.y - center.y);
                return ld < rd;
            });
        }

        ~TasksManager() noexcept
        {
            delete[] tasks;
        }

        bool empty() const noexcept
        {
            return next_task >= tasks_stop;
        }

        index2_t take() noexcept
        {
            return *(next_task++);
        }
    };


    static constexpr f32 trace_info_max_bbox_count = 10;
    static constexpr f32 trace_info_max_face_count = 10;
    static constexpr f32 trace_info_max_trace_count = 2;

    static constexpr f32 display_framerate = 30;


    Scence const& _scence;

public:

    RenderConfig config;

    Renderer(Scence const& scence_) noexcept
    : _scence{scence_}, config{SampleType::Random, 0, 0, 0} {}
    Renderer(Scence const& scence_, RenderConfig config_) noexcept
    : _scence{scence_}, config{config_} {}

    RGB render_pixel(vec2g const& pixel_center, vec2g const& pixel_size) const noexcept
    {
        RGB pixel_color = consts<RGB>::Black;

        for (u32 k = 0; k < config.rays_pre_pixel; k++)
        {
            vec2g position = pixel_center + pixel_size * (sampler.get() - fg(0.5));
            pixel_color += render_screen(position);
        }
        return pixel_color / f32(config.rays_pre_pixel);
    }
    RGB render_screen(vec2g const& position) const noexcept
    {
        using glm::normalize;
        [[maybe_unused]] VEC_CONST RGB trace_info_scaler = RGB(trace_info_max_bbox_count, trace_info_max_face_count, trace_info_max_trace_count);

        Ray ray = _scence.camera_ref().cast_ray(position);
        TraceRecord rec;
        u32 bounds = 0;
        RGB received;

        while (true)
        {
            // tracing ray
            rec.reset();
            _scence.trace(ray, rec);
#ifdef NYASRT_SHOW_TRACE_INFO
            rec.trace_count++;
#endif

            // directly render lights on screen
            if (bounds == 0) for (auto const& light_p : _scence.light_ps)
            {
                if (light_p->test_hit(ray, rec.max_ray_time))
                {
                    rec.ray_color += light_p->light(ray);
                }
            }

            if ((bounds < config.max_ray_bounds) && rec.hit_object())
            {
                // get surface material color
                RGB surface_color = (*rec.object_p->material_p)(ray, rec);

                // next bounds direction
                rec.hit_normal = normalize(rec.hit_normal);
                auto [outgoing, reflected] = rec.object_p->brdf_p->bounds(surface_color, ray, rec);

                // render object surface
                received = rec.object_p->brdf_p->emitted(surface_color, ray, rec);

                // render_lights
                Ray light_ray; light_ray.origin = rec.hit_point;
                for (auto const& light_p : _scence.light_ps)
                {
                    auto [direction, max_light_ray_time] = light_p->sample(light_ray.origin);
                    light_ray.direction = direction;

                    f32 l_dot_n = dot(light_ray.direction, rec.hit_normal);
                    if ((l_dot_n > 0) && !_scence.test_hit(light_ray, max_light_ray_time))
                    {
                        RGB surface_brdf = (*rec.object_p->brdf_p)(surface_color, light_ray.direction, -ray.direction, rec.hit_normal);
                        received += surface_brdf * l_dot_n * light_p->light(light_ray);
                    }
#ifdef NYASRT_SHOW_TRACE_INFO
                    rec.trace_count++;
#endif
                }

                // ray bounds
                rec.ray_color += rec.reflect_color * received;
                rec.reflect_color *= reflected;
                ray.origin = rec.hit_point;
                ray.direction = outgoing;
                bounds++;
            }
            else
            {
                // render sky
                if (!rec.hit_object() && _scence.has_sky())
                {
                    rec.ray_color += rec.reflect_color * _scence.sky_ref()(ray.direction);
                }
#ifdef NYASRT_SHOW_TRACE_INFO
                return RGB(rec.box_count, rec.triangle_count, rec.trace_count) / trace_info_scaler;
#endif
                return rec.ray_color;
            }
        }
    }

    void render(GraphicsBuffer & gbuf) const noexcept
    {
        if (!_scence.prepared()) { return; }
        sampler.init(config.sample_type, config.n_sample_sets, config.rays_pre_pixel);

#if defined(NYASRT_DISPLAY_PROGRESS)
        using namespace std::chrono;

        gbuf.fill(consts<RGB>::Black);
        DisplayWindow window(gbuf);
        window.refresh();

        constexpr auto refresh_interval = microseconds(u64(1e6 / display_framerate));
        auto next_refresh_time = system_clock::now() + refresh_interval;
#endif

        vec2g pixel_size = gbuf.pixel_size();

        for (SubBuffer iterator(gbuf); auto & iter : iterator)
        {
            vec2g center = gbuf.position(iter.global_index());
            iter.pixel() = render_pixel(center, pixel_size);

#if defined(NYASRT_DISPLAY_PROGRESS)
            if (system_clock::now() >= next_refresh_time)
            {
                window.refresh();
                next_refresh_time += refresh_interval;
            }
#endif
        }

#if defined(NYASRT_DISPLAY_PROGRESS)
        // wait for window closed
        /* while (!window.should_close())
        {
            window.refresh();
            std::this_thread::sleep_for(refresh_interval);
        } */
#endif
    }
    void render(GraphicsBuffer & gbuf, u32 n_threads) const
    {
        if (!_scence.prepared()) { return; }

#if defined(NYASRT_DISPLAY_PROGRESS)
        using namespace std::chrono;

        gbuf.fill(consts<RGB>::Black);
        DisplayWindow window(gbuf);
        window.refresh();

        constexpr auto refresh_interval = microseconds(u64(1e6 / display_framerate));
        auto next_refresh_time = system_clock::now() + refresh_interval;
#endif

        std::vector<bool> threads_done;
        std::vector<std::thread> threads;
        threads_done.reserve(n_threads);
        threads.reserve(n_threads);

        vec2g pixel_size = gbuf.pixel_size();
        TasksManager manager(gbuf);
        std::mutex request_task;

        for (u32 k = 0; k < n_threads; k++)
        {
            threads_done.emplace_back(false);
            threads.emplace_back([&, this, k] () noexcept
            {
                sampler.init(config.sample_type, config.n_sample_sets, config.rays_pre_pixel);
                index2_t task_start;

                while (true)
                {
                    /* requesting task */ {
                        std::lock_guard<std::mutex> lock(request_task);
                        if (manager.empty()) { break; }

                        task_start = manager.take();
                    }

                    SubBuffer iterator(gbuf, task_start, task_size);
                    iterator.clamp();
                    for (auto & iter : iterator)
                    {
                        vec2g center = gbuf.position(iter.global_index());
                        iter.pixel() = render_pixel(center, pixel_size);
                    }
                }

                threads_done[k] = true;
            });
        }

        u32 thread_index = 0;
        while (thread_index < n_threads)
        {
            if (threads_done[thread_index])
            {
                threads[thread_index].join();
                thread_index++;
            }

#if defined(NYASRT_DISPLAY_PROGRESS)
            if (system_clock::now() >= next_refresh_time)
            {
                window.refresh();
                next_refresh_time += refresh_interval;
            }
#endif
        }
#if defined(NYASRT_DISPLAY_PROGRESS)
        // wait for window closed
        /* while (!window.should_close())
        {
            window.refresh();
            std::this_thread::sleep_for(refresh_interval);
        } */
#endif
    }
};

#if (!GLM_HAS_CONSTEXPR)
index2_t const Renderer::task_size = index2_t(16, 16);
#endif

} // namespace nyasRT
