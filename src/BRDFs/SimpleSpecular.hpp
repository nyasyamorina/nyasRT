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


    virtual CONST_FUNC std::tuple<normal3g, RGB> bounds(RGB const& base_color, Ray const& ray, TraceRecord const& rec) const noexcept override
    {
        if (dot(ray.direction, rec.face_normal) > 0) // ray hit surface from behind
        {
            return {ray.direction, defaults<RGB>::White};   // surface cannot be seen from behind
        }

        // sample on upper hemisphere with distribution `dΩ = cosθ*dθdφ`
        normal3g outgoing = Sampler::sphere(random.uniform01<vec2g>());
        RGB surface_color = defaults<RGB>::White;

        if (random.uniform01<f32>() > _clearcoat)
        {
            normal3g prefect_reflection = reflect(ray.direction, rec.hit_normal);
            outgoing = normalize(mix(prefect_reflection, outgoing, _roughness));
            surface_color = base_color;
        }

        if (dot(outgoing, rec.hit_normal) < 0) { outgoing.reflect(rec.hit_normal); }
        //if (dot(outgoing, rec.face_normal) < 0) { outgoing.reflect(rec.face_normal); }
        return {outgoing, surface_color * dot(outgoing, rec.hit_normal)};
    }

    virtual CONST_FUNC RGB emitted(RGB const& base_color, Ray const& ray, TraceRecord const& rec) const noexcept override
    {
        return _emittion;
    }

    virtual CONST_FUNC RGB operator () (RGB const& base_color, normal3g const& incoming, normal3g const& outgoing, normal3g const& normal) const noexcept override
    {
        // yeah, this BRDF model produces incorrect specular shape.

        RGB reflected_color = mix(base_color, defaults<RGB>::White, _clearcoat);
        f32 pdf = 0;

        normal3g prefect_reflection = -reflect(outgoing, normal);
        vec3g C = (1 - _roughness) * prefect_reflection;
        f32 r2 = sqr(_roughness);
        f32 r_pdf_sphere = 1 / (4 * defaults<fg>::pi * _roughness);

        // the probability density of outgoing not being reflected back to the upper hemisphere
        {
            f32 t = dot(incoming, C);
            f32 d2 = length2(t * incoming - C);
            if (d2 < r2)
            {
                f32 s = std::sqrt(r2 - d2);
                pdf += r_pdf_sphere * sqr(t + s) / s;
                if (f32 tmp = t - s; tmp > 0) { pdf += r_pdf_sphere * sqr(tmp) / s; }
            }
        }
        // the probability density of light being reflected back to the upper hemisphere
        {
            normal3g r_incoming = reflect(incoming, normal);
            f32 t = dot(r_incoming, C);
            f32 d2 = length2(t * r_incoming - C);
            if (d2 < r2)
            {
                f32 s = std::sqrt(r2 - d2);
                pdf += r_pdf_sphere * sqr(t + s) / s;
                if (f32 tmp = t - s; tmp > 0) { pdf += r_pdf_sphere * sqr(tmp) / s; }
            }
        }

        pdf = mix(pdf, defaults<f32>::inv_pi, _clearcoat);
        return pdf * reflected_color;
    }
};