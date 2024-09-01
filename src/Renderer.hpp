#pragma once

#include <algorithm>
#include <math.h>
#include <mutex>
#include <thread>
#include <vector>

#include "utils.hpp"
#include "RGB.hpp"
#include "geometry/Ray.hpp"
#include "Object3D.hpp"
#include "cameras/Camera.hpp"
#include "sky_models/Sky.hpp"
#include "Figure.hpp"
#include "random.hpp"
#include "Scence.hpp"


class RenderConfig
{
public:

    u32 rays_pre_pixel, max_ray_bounds;
};

class Renderer
{
protected:

    static constexpr vec2<u32> task_size = vec2(16, 16);

    class TasksManager final
    {
    private:

        vec2<i32> * tasks;
        vec2<i32> * next_task;
        vec2<i32> * tasks_stop;

    public:

        TasksManager(Figure & fig)
        : tasks{nullptr}, next_task{nullptr}, tasks_stop{nullptr} {
            vec2<i32> center = fig.size() >> 1;
            vec2<i32> start_start = center % task_size;
            if (start_start.x > 0) { start_start.x -= task_size.x; }
            if (start_start.y > 0) { start_start.y -= task_size.y; }

            u32 total_tasks = prod((fig.size() - 1 - start_start) / task_size + 1);
            tasks = next_task = new vec2<i32>[total_tasks];
            tasks_stop = tasks + total_tasks;

            for (i64 start_y = start_start.y; start_y < fig.height(); start_y += task_size.y)
            {
                for (i64 start_x = start_start.x; start_x < fig.width(); start_x += task_size.x)
                {
                    *(next_task++) = vec2<i32>(start_x, start_y);
                }
            }
            next_task = tasks;

            std::sort(tasks, tasks_stop, [center] (vec2<i32> l, vec2<i32> r) noexcept -> bool
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

        vec2<i32> take() noexcept
        {
            return *(next_task++);
        }
    };


    static constexpr RGB trace_info_scaler = RGB(2000, 2000, 10);

    Scence const& _scence;

public:

    RenderConfig config;

    Renderer(Scence const& scence_) noexcept
    : _scence{scence_}, config{0, 0} {}
    Renderer(Scence const& scence_, RenderConfig config_) noexcept
    : _scence{scence_}, config{config_} {}
    Renderer(Scence const& scence_, u32 rays_pre_pixel, u32 max_ray_bounds) noexcept
    : _scence{scence_}, config{rays_pre_pixel, max_ray_bounds} {}

    RGB render_pixel(vec2g const& pixel_center, vec2g const& pixel_size) const noexcept
    {
        RGB pixel_color = defaults<RGB>::Black;

        for (u32 k = 0; k < config.rays_pre_pixel; k++)
        {
            vec2g position = pixel_center + pixel_size * (random.uniform01<vec2g>() - static_cast<fg>(0.5));
            pixel_color += render_screen(position);
        }
        return pixel_color.div(config.rays_pre_pixel);
    }
    RGB render_screen(vec2g const& position) const noexcept
    {
        Ray ray = _scence.camera_ref().cast_ray(position);  // ray.direction has been normalized
        TraceRecord rec;
        u32 bounds = 0;

        while (true)
        {
            // tracing ray
            rec.reset();
            for (Object3D const& object : _scence.objects)
            {
                object.trace(ray, rec);
            }

            if ((bounds < config.max_ray_bounds) && rec.hit_object())
            {
                // get surface texture color
                RGB surface_color = (*rec.object_p->texture_p)(ray, rec);

                // next bounds direction
                rec.hit_normal.normalize();
                auto [outgoing, reflected] = rec.object_p->brdf_p->bounds(surface_color, ray, rec);

                // render object surface
                rec.ray_color += rec.reflect_color * rec.object_p->brdf_p->emitted(surface_color, ray, rec);
                rec.reflect_color *= reflected;

                // ray bounds
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
#ifdef SHOW_TRACE_INFO
                rec.ray_color = RGB(rec.box_count, rec.triangle_count, bounds).div(trace_info_scaler);
#endif
                break;
            }
        }
        return rec.ray_color;
    }

    void render(Figure & fig) const noexcept
    {
        if (!_scence.prepared()) { return; }

        vec2g pixel_size = fig.pixel_size();
        ViewedFigure view(fig);

        auto stop = view.end();
        for (auto pixel = view.begin(); pixel < stop; pixel++)
        {
            vec2g center = fig.screen_position(pixel.index);
            *pixel = render_pixel(center, pixel_size);
        }
    }
    void render(Figure & fig, u32 n_threads) const
    {
        if (!_scence.prepared()) { return; }

        std::vector<std::thread> threads;
        threads.reserve(n_threads);

        vec2g pixel_size = fig.pixel_size();
        TasksManager manager(fig);
        std::mutex request_task;

        for (u32 k = 0; k < n_threads; k++)
        {
            threads.emplace_back([&, this] () noexcept
            {
                vec2<i32> task_start;
                ViewedFigure view(fig);

                while (true)
                {
                    /* requesting task */ {
                        std::lock_guard<std::mutex> lock(request_task);
                        if (manager.empty()) { break; }

                        task_start = manager.take();
                    }
                    view.set_start(task_start, task_size);

                    auto stop = view.end();
                    for (auto pixel = view.begin(); pixel < stop; pixel++)
                    {
                        vec2g center = fig.screen_position(pixel.index);
                        *pixel = render_pixel(center, pixel_size);
                    }
                }
            });
        }

        for (std::thread & thread : threads)
        {
            thread.join();
        }
    }
};
