#pragma once

#include <numeric>

#include "../utils.hpp"
#include "vec3.hpp"
#include "../RGB.hpp"


class Ray
{
public:

    vec3g origin, direction;

    CONST_FUNC Ray() noexcept
    : origin(defaults<vec3g>::O), direction{defaults<vec3g>::Z} {}
    explicit CONST_FUNC Ray(vec3g const& origin_, vec3g const& direction_) noexcept
    : origin(origin_), direction(direction_) {}

    CONST_FUNC vec3g at(fg time) const noexcept
    {
        return origin + time * direction;
    }
};


class Triangle; class Mesh; class Object3D;

class TraceRecord
{
public:

    RGB ray_color, reflect_color;
    fg max_ray_time;
    vec3g hit_point, normal;
    vec2g hit_local;
    Triangle const* triangle_p;
    Mesh const* mesh_p;
    Object3D const* object_p;

    CONST_FUNC TraceRecord() noexcept
    : ray_color{defaults<RGB>::Black}, reflect_color{defaults<RGB>::White}
    , max_ray_time{std::numeric_limits<fg>::max()}, hit_point{defaults<vec3g>::O}, normal{defaults<vec3g>::Z}
    , hit_local{defaults<vec2g>::O}, triangle_p{nullptr}, mesh_p{nullptr}, object_p{nullptr} {}

    CONST_FUNC TraceRecord & reset() noexcept
    {
        max_ray_time = std::numeric_limits<fg>::max();
        triangle_p = nullptr;
        mesh_p = nullptr;
        object_p = nullptr;
        return *this;
    }
};
