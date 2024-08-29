#pragma once

#include <memory>
#include <tuple>

#include "../utils.hpp"
#include "../geometry/vec3.hpp"
#include "../RGB.hpp"
#include "../geometry/Ray.hpp"
#include "../random.hpp"
#include "../Sampler.hpp"


class BRDF
{
public:

    virtual bool prepare() noexcept
    {
        return true;
    }

    virtual CONST_FUNC std::tuple<vec3g, RGB> bounds(RGB const& base_color, Ray const& ray, TraceRecord const& rec) const noexcept
    {
        // sample on upper hemisphere with distribution `dΩ = cosθ*dθdφ`
        if (dot(ray.direction, rec.face_normal) > 0) // ray hit surface from behind
        {
            return {ray.direction, defaults<RGB>::White};   // surface cannot be seen from behind
        }
        vec3g outgoing = rec.interpolated_normal + Sampler::sphere(random.uniform01<vec2g>());
        if (fg lo = length(outgoing); lo >= defaults<fg>::eps) { outgoing *= 1 / lo; }
        else { outgoing = rec.interpolated_normal; }

        //if (dot(outgoing, rec.face_normal) < 0) { outgoing.reflect(rec.face_normal); }
        return {outgoing, (*this)(base_color, -ray.direction, outgoing, rec.interpolated_normal)};
    }

    virtual CONST_FUNC RGB emitted(RGB const& base_color, Ray const& ray, TraceRecord const& rec) const noexcept
    {
        return defaults<RGB>::Black;
    }

    virtual RGB operator () (RGB const& base_color, vec3g const& NORMALIZED incoming, vec3g const& NORMALIZED outgoing, vec3g const& NORMALIZED normal) const noexcept = 0;
};

using BRDFPtr = std::shared_ptr<BRDF>;
using BRDFConstPtr = std::shared_ptr<BRDF const>;
