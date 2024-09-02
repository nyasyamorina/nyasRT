#pragma once

#include <math.h>

#include "../utils.hpp"
#include "vec3.hpp"
#include "vec4.hpp"
#include "Ray.hpp"


class Rotation;
CONST_FUNC Rotation operator * (Rotation const& second, Rotation const& first) noexcept;

class Rotation final
{
public:

    normal4g quaternion;

    CONST_FUNC Rotation() noexcept
    : quaternion{defaults<vec4g>::W} {}
    CONST_FUNC Rotation(normal4g const& q) noexcept
    : quaternion{q} {}
    CONST_FUNC Rotation(normal3g const& axis, fg angle) noexcept
    : quaternion{std::sin(defaults<fg>::half * angle) * axis, std::cos(defaults<fg>::half * angle)} {}
    CONST_FUNC Rotation(fg yaw, fg pitch, fg roll) noexcept
    {
        *this = Rotation(defaults<vec3g>::Z, yaw) * Rotation(defaults<vec3g>::Y, pitch) * Rotation(defaults<vec3g>::X, roll);
    }


    CONST_FUNC Rotation & operator *= (Rotation const& rot) noexcept
    {
        quaternion = qmul(rot.quaternion, quaternion);
        return *this;
    }
    CONST_FUNC Rotation & operator ^= (fg s) noexcept
    {
        vec3g axis = quaternion.xyz();
        fg len = length(axis);
        if (len < defaults<fg>::eps) { return *this; }
        fg half_angle = std::atan2(len, quaternion.w) * s;
        quaternion = vec4g((std::sin(half_angle) / len) * axis, std::cos(half_angle));
        return *this;
    }

    CONST_FUNC Rotation & mul(Rotation const& rot) noexcept
    {
        return ((*this) *= rot);
    }

    CONST_FUNC normal3g axis() const noexcept
    {
        return quaternion.xyz().normalize();
    }
    CONST_FUNC fg angle() const noexcept
    {
        return 2 * std::atan2(length(quaternion.xyz()), quaternion.w);
    }

    CONST_FUNC Rotation inverse() const noexcept
    {
        return Rotation(qconj(quaternion));
    }

    CONST_FUNC vec3g apply(vec3g const& v) const noexcept
    {
        return qmul(qmul(quaternion, vec4g(v, 0)), qconj(quaternion)).xyz();
    }
    CONST_FUNC vec3g undo(vec3g const& v) const noexcept
    {
        return qmul(qmul(qconj(quaternion), vec4g(v, 0)), quaternion).xyz();
    }
};

CONST_FUNC Rotation operator + (Rotation const& rot) noexcept
{
    return rot;
}
CONST_FUNC Rotation operator - (Rotation const& rot) noexcept
{
    return Rotation(qconj(rot.quaternion));
}
CONST_FUNC Rotation operator * (Rotation const& second, Rotation const& first) noexcept
{
    return Rotation(qmul(second.quaternion, first.quaternion));
}
CONST_FUNC Rotation operator ^ (Rotation const& rot, fg s) noexcept
{
    Rotation new_rot;
    vec3g axis = rot.quaternion.xyz();
    fg len = length(axis);
    if (len < defaults<fg>::eps) { return new_rot; }
    fg half_angle = std::atan2(len, rot.quaternion.w) * s;
    new_rot.quaternion = vec4g((std::sin(half_angle) / len) * axis, std::cos(half_angle));
    return new_rot;
}


// transform a `Mesh` from model space to world space
class Transform final
{
public:

    Rotation rotation;
    fg scaler;
    vec3g offset;

    CONST_FUNC Transform() noexcept
    : rotation{}, scaler{1}, offset{defaults<vec3g>::O} {}

    CONST_FUNC Transform & rotate(normal3g const& axis, fg angle) noexcept
    {
        return rotate(Rotation(axis, angle));
    }
    CONST_FUNC Transform & rotate(Rotation const& rot) noexcept
    {
        rotation *= rot;
        offset = rot.apply(offset);
        return *this;
    }
    CONST_FUNC Transform & scale(fg scale_) noexcept
    {
        scaler *= scale_;
        offset *= scale_;
        return *this;
    }
    CONST_FUNC Transform & shift(vec3g const& offset_) noexcept
    {
        offset += offset_;
        return *this;
    }

    CONST_FUNC Transform inverse() const noexcept
    {
        Transform inv;
        inv.rotation = rotation.inverse();
        inv.scaler = 1 / scaler;
        inv.offset = -offset;
        return inv;
    }

    CONST_FUNC vec3g apply_normal(vec3g const& n) const noexcept
    {
        return rotation.apply(n);
    }
    CONST_FUNC vec3g undo_normal(vec3g const& n) const noexcept
    {
        return rotation.undo(n);
    }
    CONST_FUNC vec3g apply_vector(vec3g const& v) const noexcept
    {
        return apply_normal(v).mul(scaler);
    }
    CONST_FUNC vec3g undo_vector(vec3g const& v) const noexcept
    {
        return undo_normal(v).div(scaler);
    }
    CONST_FUNC vec3g apply_point(vec3g const& p) const noexcept
    {
        return apply_vector(p).add(offset);
    }
    CONST_FUNC vec3g undo_point(vec3g const& p) const noexcept
    {
        return undo_vector(p - offset);
    }

    CONST_FUNC Ray apply(Ray const& ray) const noexcept
    {
        return Ray(apply_point(ray.origin), apply_vector(ray.direction));
    }
    CONST_FUNC Ray undo(Ray const& ray) const noexcept
    {
        return Ray(undo_point(ray.origin), undo_vector(ray.direction));
    }
};
