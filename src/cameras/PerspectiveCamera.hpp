#pragma once

#include <math.h>

#include "../utils.hpp"
#include "../geometry/vec3.hpp"
#include "../geometry/Ray.hpp"
#include "../geometry/Transform.hpp"
#include "Camera.hpp"


class PerspectiveCamera : public Camera
{
private:

    Ray _view;
    vec3g _horizontal, _vertical;

public:

    PerspectiveCamera() noexcept
    : _view{defaults<vec3g>::O, defaults<vec3g>::X}, _horizontal{-defaults<vec3g>::Y}, _vertical{defaults<vec3g>::Z} {}
    virtual ~PerspectiveCamera() noexcept = default;


    virtual Ray cast_ray(vec2g const& screen_position) const noexcept override
    {
        vec3g direction = _view.direction + screen_position.x * _horizontal + screen_position.y * _vertical;
        return Ray(_view.origin, direction.normalize());
    }


    // can be used in `view_direction(view_direction, view_up)`
    static CONST_FUNC vec3g choose_view_up(vec3g const& view_direction_) noexcept
    {
        if ((std::abs(view_direction_.z) > defaults<fg>::eps) &&
            (std::abs(view_direction_.x / view_direction_.z) < defaults<fg>::eps) &&
            (std::abs(view_direction_.y / view_direction_.z) < defaults<fg>::eps))
        {
            return (std::signbit(view_direction_.z) ? static_cast<fg>(1) : static_cast<fg>(-1)) * defaults<vec3g>::Y;
        }
        else
        {
            return defaults<vec3g>::Z;
        }
    }


    /******** setters ********/

    PerspectiveCamera & view_origin(vec3g const& view_origin_) noexcept
    {
        _view.origin = view_origin_;
        return *this;
    }
    PerspectiveCamera & view_angle(fg yaw, fg pitch) noexcept
    {
        return view_angle(yaw, pitch, static_cast<fg>(0));
    }
    PerspectiveCamera & view_angle(fg yaw, fg pitch, fg roll) noexcept
    {
        Rotation rot(yaw, pitch, roll);

        _view.direction = length(_view.direction) * rot.apply( defaults<vec3g>::X);
        _horizontal     = length(_horizontal)     * rot.apply(-defaults<vec3g>::Y);
        _vertical       = length(_vertical)       * rot.apply( defaults<vec3g>::Z);

        return *this;
    }
    PerspectiveCamera & view_direction(vec3g const& view_direction_) noexcept
    {
        return view_direction(view_direction_, _vertical);
    }
    PerspectiveCamera & view_direction(vec3g const& view_direction_, vec3g const& view_up) noexcept
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
    PerspectiveCamera & view_plane_distance(fg view_plane_distance_) noexcept
    {
        fg multiplier = view_plane_distance_ / length(_view.direction);
        _view.direction *= multiplier;
        _horizontal *= multiplier;
        _vertical *= multiplier;
        return *this;
    }
    PerspectiveCamera & field_of_view(fg fov) noexcept
    {
        fg n = std::tan(static_cast<fg>(0.5) * fov) * length(_view.direction) / length(_horizontal);
        _horizontal *= n; _vertical *= n;
        return *this;
    }
    PerspectiveCamera & aspect_ratio(fg aspect_ratio_) noexcept
    {
        _vertical *= length(_horizontal) / (aspect_ratio_ * length(_vertical));
        return *this;
    }


    /******** getters ********/

    vec3g const& view_origin() const noexcept
    {
        return _view.origin;
    }
    vec3g const& view_direction() const noexcept
    {
        return _view.direction;
    }
    vec3g const& horizontal() const noexcept
    {
        return _horizontal;
    }
    vec3g const& vertical() const noexcept
    {
        return _vertical;
    }
    fg view_plane_distance() const noexcept
    {
        return length(_view.direction);
    }
    fg field_of_view() const noexcept
    {
        return std::atan(length(_horizontal) / length(_view.direction));
    }
    fg aspect_ratio() const noexcept
    {
        return length(_horizontal) / length(_vertical);
    }
};
