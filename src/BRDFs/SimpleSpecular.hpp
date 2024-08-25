#pragma once

#include "BRDF.hpp"


class SimpleSpecular : public BRDF
{
protected:

    RGB _base_color, _emittion;
    f32 _roughness, _clearcoat;

public:

    CONST_FUNC SimpleSpecular() noexcept
    : _base_color{defaults<RGB>::White}, _emittion{defaults<RGB>::Black}, _roughness{0.5f}, _clearcoat{0.5f} {}
    virtual ~SimpleSpecular() noexcept = default;

    CONST_FUNC SimpleSpecular & base_color(RGB const& value) noexcept
    {
        _base_color = value;
        return *this;
    }
    CONST_FUNC SimpleSpecular & emittion(RGB const& value) noexcept
    {
        _emittion = value;
        return *this;
    }
    CONST_FUNC SimpleSpecular & roughness(f32 value) noexcept
    {
        _roughness = value;
        return *this;
    }
    CONST_FUNC SimpleSpecular & clearcoat(f32 value) noexcept
    {
        _clearcoat = value;
        return *this;
    }

    CONST_FUNC RGB const& base_color() const noexcept
    {
        return _base_color;
    }
    CONST_FUNC RGB const& emittion() const noexcept
    {
        return _emittion;
    }
    CONST_FUNC f32 roughness() const noexcept
    {
        return _roughness;
    }
    CONST_FUNC f32 clearcoat() const noexcept
    {
        return _clearcoat;
    }


    virtual CONST_FUNC std::tuple<vec3g, RGB> bounds(vec3g const& NORMALIZED incoming, vec3g & NORMALIZED normal) noexcept override
    {
        // sample on upper hemisphere with distribution `dΩ = cosθ*dθdφ`
        if (dot(incoming, normal) < 0)  // ray hit surface from behind
        {
            return {-incoming, defaults<RGB>::White};   // surface cannot be seen from behind
            //normal.neg();
        }
        vec3g diffuse = normal + Sampler::sphere(random.uniform01<vec2g>());
        if (fg lo = length(diffuse); lo >= defaults<fg>::eps) { diffuse *= 1 / lo; }
        else { diffuse = normal; }

        bool ray_hit_clearcoat = random.uniform01<f32>() < 0.25f * _clearcoat;
        if (ray_hit_clearcoat) { return {diffuse, defaults<RGB>::White}; }

        vec3g prefect_reflection = -reflect(incoming, normal);
        vec3g outgoing = normalize(mix(prefect_reflection, diffuse, _roughness));
        return {outgoing, _base_color};
    }

    virtual CONST_FUNC RGB emitted(vec3g const& NORMALIZED incoming, vec3g & NORMALIZED normal) const noexcept override
    {
        return _emittion;
    }

    virtual CONST_FUNC RGB operator () (vec3g const& NORMALIZED incoming, vec3g const& NORMALIZED outgoing, vec3g const& NORMALIZED normal) const noexcept override
    {
        // unknown BRDF
        return RGB(-1.0f);
    }
};