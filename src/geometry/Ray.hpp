#pragma once

#include "../common.hpp"


namespace nyasRT
{
class Ray
{
public:

    vec3g origin;
    normal3g direction; // should be normalized in world space to calculate BRDF & Sky

    VEC_CONSTEXPR Ray() noexcept
    : origin{consts<vec3g>::O}, direction{consts<normal3g>::X} {}
    VEC_CONSTEXPR Ray(vec3g const& origin_, normal3g const& direction_) noexcept
    : origin{origin_}, direction{direction_} {}

    VEC_CONSTEXPR vec3g at(fg time) const noexcept
    {
        return origin + time * direction;
    }
};


class Object3D;

class TraceRecord
{
public:

    RGB ray_color, reflect_color;
    fg max_ray_time;
    vec3g hit_point;
    normal3g face_normal, hit_normal;
    vec2g hit_face, hit_texture;
    Object3D const* object_p;
#if defined(NYASRT_SHOW_TRACE_INFO)
    u32 box_count, triangle_count, trace_count;
#endif

    VEC_CONSTEXPR TraceRecord() noexcept
    : ray_color{consts<RGB>::Black}, reflect_color{consts<RGB>::White}
    , max_ray_time{consts<fg>::inf} , object_p{nullptr}
#if defined(NYASRT_SHOW_TRACE_INFO)
    , box_count{0}, triangle_count{0}, trace_count{0}
#endif
    {}

    VEC_CONSTEXPR TraceRecord & reset() noexcept
    {
        max_ray_time = consts<fg>::inf;
        object_p = nullptr;
        return *this;
    }

    constexpr bool hit_object() const noexcept
    {
        return object_p != nullptr;
    }
};

} // namespace nyasRT