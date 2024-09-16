#pragma once

#include <math.h>
#include <numeric>
#include <tuple>

#include "../common.hpp"
#include "Ray.hpp"


namespace nyasRT
{
class BoundingBox
{
    static inline constexpr fg sign(fg x) noexcept
    {
        return x < 0 ? -1 : (x > 0 ? 1 : 0);
    }

public:

    vec3g min_corner, max_corner;

    VEC_CONSTEXPR BoundingBox() noexcept
    : min_corner{consts<vec3g>::max}, max_corner{consts<vec3g>::min} {}

    constexpr bool prepare() noexcept
    {
        constexpr fg big_eps = consts<fg>::eps;

        if (std::abs(max_corner.x - min_corner.x) < consts<fg>::eps)
        {
            min_corner.x = min_corner.x * (1 - big_eps * sign(min_corner.x)) - big_eps;
            max_corner.x = max_corner.x * (1 + big_eps * sign(max_corner.x)) + big_eps;
        }
        if (std::abs(max_corner.y - min_corner.y) < consts<fg>::eps)
        {
            min_corner.y = min_corner.y * (1 - big_eps * sign(min_corner.y)) - big_eps;
            max_corner.y = max_corner.y * (1 + big_eps * sign(max_corner.y)) + big_eps;
        }
        if (std::abs(max_corner.z - min_corner.z) < consts<fg>::eps)
        {
            min_corner.z = min_corner.z * (1 - big_eps * sign(min_corner.z)) - big_eps;
            max_corner.z = max_corner.z * (1 + big_eps * sign(max_corner.z)) + big_eps;
        }
        return true;
    }
    VEC_CONSTEXPR BoundingBox & reset() noexcept
    {
        min_corner = consts<vec3g>::max;
        max_corner = consts<vec3g>::min;
        return *this;
    }
    VEC_CONSTEXPR BoundingBox & bound(vec3g const& point) noexcept
    {
        using glm::min, glm::max;
        min_corner = min(min_corner, point);
        max_corner = max(max_corner, point);
        return *this;
    }

    VEC_CONSTEXPR vec3g size() const noexcept
    {
        return max_corner - min_corner;
    }

    static constexpr inline bool intersect(fg time_in, fg time_out, fg max_ray_time) noexcept
    {
        return (time_out >= consts<fg>::eps) && (time_in < max_ray_time) && (time_in < time_out);
    }

    VEC_CONSTEXPR std::tuple<fg, fg> trace(Ray const& ray) const noexcept
    {
        vec3g inv_d = fg(1) / ray.direction;
        vec3g min_time = (min_corner - ray.origin) * inv_d;
        vec3g max_time = (max_corner - ray.origin) * inv_d;

        if (inv_d.x < 0) { std::swap(min_time.x, max_time.x); }
        if (inv_d.y < 0) { std::swap(min_time.y, max_time.y); }
        if (inv_d.z < 0) { std::swap(min_time.z, max_time.z); }

        fg time_in  = std::max(std::max(min_time.x, min_time.y), min_time.z);
        fg time_out = std::min(std::min(max_time.x, max_time.y), max_time.z);
        return {time_in, time_out};
    }
    VEC_CONSTEXPR bool trace(Ray const& ray, TraceRecord const& rec) const noexcept
    {
        auto [time_in, time_out] = trace(ray);
        return intersect(time_in, time_out, rec.max_ray_time);
    }
};

} // namespace nyasRT
