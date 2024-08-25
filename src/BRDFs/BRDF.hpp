#pragma once

#include <memory>
#include <tuple>

#include "../utils.hpp"
#include "../geometry/vec3.hpp"
#include "../RGB.hpp"
#include "../random.hpp"
#include "../Sampler.hpp"


class BRDF
{
public:

    virtual bool prepare() noexcept
    {
        return true;
    }

    virtual CONST_FUNC std::tuple<vec3g, RGB> bounds(vec3g const& NORMALIZED incoming, vec3g & NORMALIZED normal) noexcept
    {
        // sample on upper hemisphere with distribution `dΩ = cosθ*dθdφ`
        if (dot(incoming, normal) < 0)  // ray hit surface from behind
        {
            return {-incoming, defaults<RGB>::White};   // surface cannot be seen from behind
        }
        vec3g outgoing = normal + Sampler::sphere(random.uniform01<vec2g>());
        if (fg lo = length(outgoing); lo >= defaults<fg>::eps) { outgoing *= 1 / lo; }
        else { outgoing = normal; }

        return {outgoing, (*this)(incoming, outgoing, normal)};
    }

    virtual CONST_FUNC RGB emitted(vec3g const& NORMALIZED incoming, vec3g & NORMALIZED normal) const noexcept
    {
        return defaults<RGB>::Black;
    }

    virtual RGB operator () (vec3g const& NORMALIZED incoming, vec3g const& NORMALIZED outgoing, vec3g const& NORMALIZED normal) const noexcept = 0;
};

using BRDFPtr = std::shared_ptr<BRDF>;
using BRDFConstPtr = std::shared_ptr<BRDF const>;
