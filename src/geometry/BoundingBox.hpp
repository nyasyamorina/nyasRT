#pragma once

#include <math.h>
#include <numeric>
#include <tuple>

#include "../utils.hpp"
#include "vec3.hpp"
#include "Ray.hpp"


class BoundingBox
{
public:

    vec3g min_corner, max_corner;

    CONST_FUNC BoundingBox() noexcept
    : min_corner{defaults<vec3g>::max}, max_corner{defaults<vec3g>::min} {}

    CONST_FUNC bool prepare() noexcept
    {
        vec3<bool> thin_layer = (max_corner / min_corner).isleq(defaults<fg>::eps);
        if (thin_layer.x)
        {
            min_corner.x *= 1 - defaults<fg>::eps;
            max_corner.x *= 1 + defaults<fg>::eps;
        }
        if (thin_layer.y)
        {
            min_corner.y *= 1 - defaults<fg>::eps;
            max_corner.y *= 1 + defaults<fg>::eps;
        }
        if (thin_layer.z)
        {
            min_corner.z *= 1 - defaults<fg>::eps;
            max_corner.z *= 1 + defaults<fg>::eps;
        }
        return true;
    }
    CONST_FUNC BoundingBox & reset() noexcept
    {
        min_corner = defaults<vec3g>::max;
        max_corner = defaults<vec3g>::min;
        return *this;
    }
    CONST_FUNC BoundingBox & bound(vec3g const& point) noexcept
    {
        min_corner = min(min_corner, point);
        max_corner = max(max_corner, point);
        return *this;
    }

    CONST_FUNC vec3g size() const noexcept
    {
        return max_corner - min_corner;
    }

    static CONST_FUNC bool intersect(fg time_in, fg time_out, fg max_ray_time) noexcept
    {
        return (time_out >= defaults<fg>::eps) && (time_in < max_ray_time) && (time_in < time_out);
    }

    CONST_FUNC std::tuple<fg, fg> trace(Ray const& ray) const noexcept
    {
        vec3g min_time = (min_corner - ray.origin) / ray.direction;
        vec3g max_time = (max_corner - ray.origin) / ray.direction;

        std::tuple<fg, fg> time_in_out{std::numeric_limits<fg>::min(), std::numeric_limits<fg>::max()};

        std::get<0>(time_in_out) = std::max(std::get<0>(time_in_out), std::signbit(ray.direction.x) ? max_time.x : min_time.x);
        std::get<1>(time_in_out) = std::min(std::get<1>(time_in_out), std::signbit(ray.direction.x) ? min_time.x : max_time.x);

        std::get<0>(time_in_out) = std::max(std::get<0>(time_in_out), std::signbit(ray.direction.y) ? max_time.y : min_time.y);
        std::get<1>(time_in_out) = std::min(std::get<1>(time_in_out), std::signbit(ray.direction.y) ? min_time.y : max_time.y);

        std::get<0>(time_in_out) = std::max(std::get<0>(time_in_out), std::signbit(ray.direction.z) ? max_time.z : min_time.z);
        std::get<1>(time_in_out) = std::min(std::get<1>(time_in_out), std::signbit(ray.direction.z) ? min_time.z : max_time.z);

        return time_in_out;
    }
    CONST_FUNC bool trace(Ray const& ray, TraceRecord const& rec) const noexcept
    {
        auto [time_in, time_out] = trace(ray);
        return intersect(time_in, time_out, rec.max_ray_time);
    }
};
