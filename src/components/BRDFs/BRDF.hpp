#pragma once

#include <tuple>

#include "../../common.hpp"
#include "../../geometry/Ray.hpp"
#include "../../Sampler.hpp"


namespace nyasRT
{
namespace BRDFs
{
class BRDF
{
public:

    virtual bool prepare() noexcept
    {
        return true;
    }

    virtual VEC_CONSTEXPR std::tuple<normal3g, RGB> bounds(RGB const& base_color, Ray const& ray, TraceRecord const& rec) const noexcept
    {
        using glm::reflect;
        // sample on upper hemisphere with distribution `dΩ = cosθ*dθdφ`
        if (dot(ray.direction, rec.face_normal) > 0) // ray hit surface from behind
        {
            return {ray.direction, consts<RGB>::White};   // surface cannot be seen from behind
        }
        vec3g outgoing = rec.hit_normal + Sampler::sphere(sampler.get());
        if (fg lo = length(outgoing); lo >= consts<fg>::eps) { outgoing *= 1 / lo; }
        else { outgoing = rec.hit_normal; }

        //if (dot(outgoing, rec.face_normal) < 0) { outgoing = reflect(outgoing, rec.face_normal); }
        return {outgoing, (*this)(base_color, -ray.direction, outgoing, rec.hit_normal)};
    }

    virtual VEC_CONSTEXPR RGB emitted(RGB const& base_color, Ray const& ray, TraceRecord const& rec) const noexcept
    {
        return consts<RGB>::Black;
    }

    virtual RGB operator () (RGB const& base_color, normal3g const& incoming, normal3g const& outgoing, normal3g const& normal) const noexcept = 0;
};

} // namespace BRDFs
} // namespace nyasRT
