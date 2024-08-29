#pragma once

#include "BRDF.hpp"


class SimpleSpecular : public BRDF
{
protected:

    RGB _emittion;
    f32 _roughness, _clearcoat;

public:

    CONST_FUNC SimpleSpecular() noexcept
    : _emittion{defaults<RGB>::Black}, _roughness{0.5f}, _clearcoat{0.5f} {}
    virtual ~SimpleSpecular() noexcept = default;

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


    virtual CONST_FUNC std::tuple<vec3g, RGB> bounds(RGB const& base_color, Ray const& ray, TraceRecord const& rec) const noexcept override
    {
        if (dot(ray.direction, rec.face_normal) > 0) // ray hit surface from behind
        {
            return {ray.direction, defaults<RGB>::White};   // surface cannot be seen from behind
        }

        // sample on upper hemisphere with distribution `dΩ = cosθ*dθdφ`
        vec3g outgoing = rec.interpolated_normal + Sampler::sphere(random.uniform01<vec2g>());
        RGB surface_color = defaults<RGB>::White;
        // normalize
        if (fg lo = length(outgoing); lo >= defaults<fg>::eps) { outgoing *= 1 / lo; }
        else { outgoing = rec.interpolated_normal; }

        if (random.uniform01<f32>() > 0.25f * _clearcoat)
        {
            vec3g prefect_reflection = reflect(ray.direction, rec.interpolated_normal);
            outgoing = normalize(mix(prefect_reflection, outgoing, _roughness));
            surface_color = base_color;
        }

        //if (dot(outgoing, rec.face_normal) < 0) { outgoing.reflect(rec.face_normal); }
        return {outgoing, surface_color};
    }

    virtual CONST_FUNC RGB emitted(RGB const& base_color, Ray const& ray, TraceRecord const& rec) const noexcept override
    {
        return _emittion;
    }

    virtual CONST_FUNC RGB operator () (RGB const& base_color, vec3g const& NORMALIZED incoming, vec3g const& NORMALIZED outgoing, vec3g const& NORMALIZED normal) const noexcept override
    {
        // unknown BRDF
        return RGB(-1.0f);
    }
};