#pragma once

#include <mutex>
#include <thread>
#include <vector>

#include "utils.hpp"
#include "RGB.hpp"
#include "geometry/Ray.hpp"
#include "Object3D.hpp"
#include "cameras/Camera.hpp"
#include "Figure.hpp"
#include "random.hpp"
#include "Scence.hpp"


class RenderConfig
{
public:

    u32 rays_pre_pixel, ray_depth;
};

class Renderer
{
protected:

    class PixelCounter
    {
    public:

        u32 row, col, width;

        PixelCounter(Figure const& fig) noexcept
        : row(0), col(0), width(fig.width()) {}

        PixelCounter & operator ++ () noexcept
        {
            if ((++row) >= width)
            {
                row = 0;
                col++;
            }
            return *this;
        }
        PixelCounter & operator ++ (i32) noexcept
        {
            return ++(*this);
        }
    };

    Scence const& _scence;

public:

    RenderConfig config;

    Renderer(Scence const& scence_) noexcept
    : _scence{scence_}, config{0, 0} {}
    Renderer(Scence const& scence_, RenderConfig config_) noexcept
    : _scence{scence_}, config{config_} {}
    Renderer(Scence const& scence_, u32 rays_pre_pixel, u32 ray_depth) noexcept
    : _scence{scence_}, config{rays_pre_pixel, ray_depth} {}

    RGB render_pixel(vec2g const& pixel_center, vec2g const& pixel_size) const noexcept
    {
        RGB pixel_color = defaults<RGB>::Black;

        TraceRecord rec;
        for (u32 k = 0; k < config.rays_pre_pixel; k++)
        {
            vec2g position = pixel_center + pixel_size * (random.uniform01<vec2g>() - static_cast<fg>(0.5));
            pixel_color += render_screen(position);
        }
        return pixel_color.div(config.rays_pre_pixel);
    }
    RGB render_screen(vec2g const& position) const noexcept
    {
        Ray ray = _scence.camera_ref().cast_ray(position);

        TraceRecord rec;
        for (Object3D const& object : _scence.objects)
        {
            object.trace(ray, rec);
        }

        RGB normal_color;
        if (rec.object_p != nullptr)
        {
            normal_color = rec.normal;
        }
        else
        {
            normal_color = ray.direction.normalize();
        }
        return normal_color.add(1).div(2);
    }

    void render(Figure & fig) const noexcept
    {
        if (!_scence.prepared()) { return; }

        vec2g pixel_size = fig.pixel_size();
        PixelCounter counter(fig);

        for (RGB & pixel : fig)
        {
            vec2g center = fig.screen_position(counter.row, counter.col);
            pixel = render_pixel(center, pixel_size);

            counter++;
        }
    }
    void render(Figure & fig, u32 n_threads) const
    {
        if (!_scence.prepared()) { return; }

        std::vector<std::thread> threads;
        threads.reserve(n_threads);

        vec2g pixel_size = fig.pixel_size();
        RGB * global_pixel = fig.begin();
        u32 golbal_col = 0;
        std::mutex request_pixel;

        for (u32 k = 0; k < n_threads; k++)
        {
            threads.emplace_back([&, this] ()
            {
                RGB * pixel;
                u32 col;

                while (true)
                {
                    /* requesting pixels */ {
                        std::lock_guard<std::mutex> lock(request_pixel);

                        if (!(golbal_col < fig.height())) { break; }

                        col = golbal_col++;
                        pixel = global_pixel;
                        global_pixel += fig.width();
                    }

                    for (u32 row = 0; row < fig.width(); row++)
                    {
                        vec2g center = fig.screen_position(row, col);
                        *(pixel++) = render_pixel(center, pixel_size);
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
