#pragma once

#include <math.h>

#include "../../geometry/Transform.hpp"
#include "Camera.hpp"


namespace nyasRT
{
namespace cameras
{
class PerspectiveCamera : public Camera
{
private:

    Ray _view;
    vec3g _horizontal, _vertical;

public:

    VEC_CONSTEXPR PerspectiveCamera() noexcept
    : _view{consts<vec3g>::O, consts<vec3g>::X}, _horizontal{-consts<vec3g>::Y}, _vertical{consts<vec3g>::Z} {}
    virtual ~PerspectiveCamera() noexcept = default;


    virtual Ray cast_ray(vec2g const& screen_position) const noexcept override
    {
        using glm::normalize;
        vec3g direction = _view.direction + screen_position.x * _horizontal + screen_position.y * _vertical;
        return Ray(_view.origin, normalize(direction));
    }


    // can be used in `view_direction(view_direction, view_up)`
    static VEC_CONSTEXPR vec3g choose_view_up(vec3g const& view_direction_) noexcept
    {
        if ((std::abs(view_direction_.z) > consts<fg>::eps) &&
            (std::abs(view_direction_.x / view_direction_.z) < consts<fg>::eps) &&
            (std::abs(view_direction_.y / view_direction_.z) < consts<fg>::eps))
        {
            return (std::signbit(view_direction_.z) ? static_cast<fg>(1) : static_cast<fg>(-1)) * consts<vec3g>::Y;
        }
        else
        {
            return consts<vec3g>::Z;
        }
    }


    /******** setters ********/

    VEC_CONSTEXPR PerspectiveCamera & view_origin(vec3g const& view_origin_) noexcept
    {
        _view.origin = view_origin_;
        return *this;
    }
    VEC_CONSTEXPR PerspectiveCamera & view_angle(fg yaw, fg pitch) noexcept
    {
        return view_angle(yaw, pitch, static_cast<fg>(0));
    }
    VEC_CONSTEXPR PerspectiveCamera & view_angle(fg yaw, fg pitch, fg roll) noexcept
    {
        Rotation rot(yaw, pitch, roll);

        _view.direction = length(_view.direction) * rot.apply( consts<vec3g>::X);
        _horizontal     = length(_horizontal)     * rot.apply(-consts<vec3g>::Y);
        _vertical       = length(_vertical)       * rot.apply( consts<vec3g>::Z);

        return *this;
    }
    VEC_CONSTEXPR PerspectiveCamera & view_direction(vec3g const& view_direction_) noexcept
    {
        return view_direction(view_direction_, _vertical);
    }
    VEC_CONSTEXPR PerspectiveCamera & view_direction(vec3g const& view_direction_, vec3g const& view_up) noexcept
    {
        fg nh = length(_horizontal) * (length(view_direction_) / length(_view.direction));
        fg nv = length(_vertical  ) * (length(view_direction_) / length(_view.direction));

        _view.direction = view_direction_;
        _horizontal = cross(view_direction_, view_up);
        _vertical   = cross(_horizontal, view_direction_);
        _horizontal *= nh / length(_horizontal);
        _vertical   *= nv / length(_vertical);

        return *this;
    }
    VEC_CONSTEXPR PerspectiveCamera & view_plane_distance(fg view_plane_distance_) noexcept
    {
        fg multiplier = view_plane_distance_ / length(_view.direction);
        _view.direction *= multiplier;
        _horizontal *= multiplier;
        _vertical *= multiplier;
        return *this;
    }
    VEC_CONSTEXPR PerspectiveCamera & field_of_view(fg fov) noexcept
    {
        fg n = std::tan(static_cast<fg>(0.5) * fov) * length(_view.direction) / length(_horizontal);
        _horizontal *= n; _vertical *= n;
        return *this;
    }
    VEC_CONSTEXPR PerspectiveCamera & aspect_ratio(fg aspect_ratio_) noexcept
    {
        _vertical *= length(_horizontal) / (aspect_ratio_ * length(_vertical));
        return *this;
    }


    /******** getters ********/

    VEC_CONSTEXPR vec3g const& view_origin() const noexcept
    {
        return _view.origin;
    }
    VEC_CONSTEXPR vec3g const& view_direction() const noexcept
    {
        return _view.direction;
    }
    VEC_CONSTEXPR vec3g const& horizontal() const noexcept
    {
        return _horizontal;
    }
    VEC_CONSTEXPR vec3g const& vertical() const noexcept
    {
        return _vertical;
    }
    constexpr fg view_plane_distance() const noexcept
    {
        return length(_view.direction);
    }
    constexpr fg field_of_view() const noexcept
    {
        return std::atan(length(_horizontal) / length(_view.direction));
    }
    constexpr fg aspect_ratio() const noexcept
    {
        return length(_horizontal) / length(_vertical);
    }
};

} // namespace cameras
} // namespace nyasRT
