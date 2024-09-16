#pragma once

#include <math.h>

#include "../common.hpp"
#include "Ray.hpp"


namespace nyasRT
{
class Rotation;
VEC_CONSTEXPR Rotation operator*(Rotation const& second, Rotation const& first) noexcept;

class Rotation final
{
public:

    using quaternion = vec4g;

    static constexpr inline fg real(quaternion const& q) noexcept
    {
        return q.w;
    }
    static VEC_CONSTEXPR inline vec3g imag(quaternion const& q) noexcept
    {
        return vec3g(q.x, q.y, q.z);
    }
    static VEC_CONSTEXPR inline quaternion qconj(quaternion const& q) noexcept
    {
        return quaternion(-imag(q), real(q));
    }
    static VEC_CONSTEXPR inline quaternion qmul(quaternion const& l, quaternion const& r) noexcept
    {
        fg const              Rl = real(l), Rr = real(r);
        ::nyasRT::vec3g const Il = imag(l), Ir = imag(r);
        return quaternion(Rl * Ir + Rr * Il + cross(Il, Ir), Rl * Rr - dot(Il, Ir));
    }


    quaternion q;

    VEC_CONSTEXPR Rotation() noexcept
    : q{consts<vec4g>::W} {}
    explicit VEC_CONSTEXPR Rotation(quaternion const& q_) noexcept
    : q{q_} {}
    VEC_CONSTEXPR Rotation(normal3g const& axis, fg angle) noexcept
    : q{std::sin(consts<fg>::half * angle) * axis, std::cos(consts<fg>::half * angle)} {}
    VEC_CONSTEXPR Rotation(fg yaw, fg pitch, fg roll) noexcept
    {
        *this = Rotation(consts<vec3g>::Z, yaw) * Rotation(consts<vec3g>::Y, pitch) * Rotation(consts<vec3g>::X, roll);
    }


    VEC_CONSTEXPR normal3g axis() const noexcept
    {
        using namespace glm;
        return normalize(imag(q));
    }
    VEC_CONSTEXPR fg angle() const noexcept
    {
        using namespace glm;
        return 2 * std::atan2(length(imag(q)), real(q));
    }

    VEC_CONSTEXPR Rotation inverse() const noexcept
    {
        return Rotation(qconj(q));
    }

    VEC_CONSTEXPR vec3g apply(vec3g const& v) const noexcept
    {
        return imag(qmul(qmul(q, vec4g(v, 0)), qconj(q)));
    }
    VEC_CONSTEXPR vec3g undo(vec3g const& v) const noexcept
    {
        return imag(qmul(qmul(qconj(q), vec4g(v, 0)), q));
    }
};

VEC_CONSTEXPR inline Rotation operator*(Rotation const& second, Rotation const& first) noexcept
{
    return Rotation(Rotation::qmul(second.q, first.q));
}
inline Rotation operator^(Rotation const& rot, fg s) noexcept
{
    using namespace glm;
    Rotation new_rot;
    vec3g const axis = Rotation::imag(rot.q);
    fg const len = length(axis);
    if (len < consts<fg>::eps) { return new_rot; }
    fg const half_angle = std::atan2(len, Rotation::real(rot.q)) * s;
    new_rot.q = vec4g((std::sin(half_angle) / len) * axis, std::cos(half_angle));
    return new_rot;
}


// transform a `Mesh` from model space to world space
class Transform final
{
public:

    Rotation rotation;
    fg scaler;
    vec3g offset;

    VEC_CONSTEXPR Transform() noexcept
    : rotation{}, scaler{1}, offset{consts<vec3g>::O} {}

    VEC_CONSTEXPR Transform & rotate(normal3g const& axis, fg angle) noexcept
    {
        return rotate(Rotation(axis, angle));
    }
    VEC_CONSTEXPR Transform & rotate(Rotation const& rot) noexcept
    {
        rotation = rot * rotation;
        offset = rot.apply(offset);
        return *this;
    }
    VEC_CONSTEXPR Transform & scale(fg scale_) noexcept
    {
        scaler *= scale_;
        offset *= scale_;
        return *this;
    }
    VEC_CONSTEXPR Transform & shift(vec3g const& offset_) noexcept
    {
        offset += offset_;
        return *this;
    }

    VEC_CONSTEXPR Transform inverse() const noexcept
    {
        Transform inv;
        inv.rotation = rotation.inverse();
        inv.scaler = 1 / scaler;
        inv.offset = -offset;
        return inv;
    }

    VEC_CONSTEXPR vec3g apply_normal(vec3g const& n) const noexcept
    {
        return rotation.apply(n);
    }
    VEC_CONSTEXPR vec3g undo_normal(vec3g const& n) const noexcept
    {
        return rotation.undo(n);
    }
    VEC_CONSTEXPR vec3g apply_vector(vec3g const& v) const noexcept
    {
        return apply_normal(v) / scaler;
    }
    VEC_CONSTEXPR vec3g undo_vector(vec3g const& v) const noexcept
    {
        return undo_normal(v) / scaler;
    }
    VEC_CONSTEXPR vec3g apply_point(vec3g const& p) const noexcept
    {
        return apply_vector(p) + offset;
    }
    VEC_CONSTEXPR vec3g undo_point(vec3g const& p) const noexcept
    {
        return undo_vector(p - offset);
    }

    VEC_CONSTEXPR Ray apply(Ray const& ray) const noexcept
    {
        return Ray(apply_point(ray.origin), apply_vector(ray.direction));
    }
    VEC_CONSTEXPR Ray undo(Ray const& ray) const noexcept
    {
        return Ray(undo_point(ray.origin), undo_vector(ray.direction));
    }
};

} // namespace nyasRT
