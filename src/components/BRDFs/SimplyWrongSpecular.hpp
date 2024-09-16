#pragma once

#include "../../PCG.hpp"
#include "BRDF.hpp"


namespace nyasRT
{
namespace BRDFs
{
class SimplyWrongSpecular : public BRDF
{
protected:

    RGB _emittion;
    f32 _roughness, _clearcoat;

public:

    VEC_CONSTEXPR SimplyWrongSpecular() noexcept
    : _emittion{consts<RGB>::Black}, _roughness{0.5f}, _clearcoat{0.5f} {}
    virtual ~SimplyWrongSpecular() noexcept = default;

    VEC_CONSTEXPR SimplyWrongSpecular & emittion(RGB const& value) noexcept
    {
        _emittion = value;
        return *this;
    }
    VEC_CONSTEXPR SimplyWrongSpecular & roughness(f32 value) noexcept
    {
        _roughness = value;
        return *this;
    }
    VEC_CONSTEXPR SimplyWrongSpecular & clearcoat(f32 value) noexcept
    {
        _clearcoat = value;
        return *this;
    }

    VEC_CONSTEXPR RGB emittion() const noexcept
    {
        return _emittion;
    }
    VEC_CONSTEXPR f32 roughness() const noexcept
    {
        return _roughness;
    }
    VEC_CONSTEXPR f32 clearcoat() const noexcept
    {
        return _clearcoat;
    }


    virtual VEC_CONSTEXPR std::tuple<normal3g, RGB> bounds(RGB const& base_color, Ray const& ray, TraceRecord const& rec) const noexcept override
    {
        using glm::reflect;
        if (dot(ray.direction, rec.face_normal) > 0) // ray hit surface from behind
        {
            return {ray.direction, consts<RGB>::White};   // surface cannot be seen from behind
        }

        // sample on upper hemisphere with distribution `dΩ = cosθ*dθdφ`
        normal3g outgoing = Sampler::sphere(sampler.get());
        RGB surface_color = consts<RGB>::White;

        if (pcg.uniform01<f32>() > _clearcoat)
        {
            normal3g prefect_reflection = reflect(ray.direction, rec.hit_normal);
            outgoing = normalize(lerp(prefect_reflection, outgoing, fg(_roughness)));
            surface_color = base_color;
        }

        if (dot(outgoing, rec.hit_normal) < 0) { outgoing = reflect(outgoing, rec.hit_normal); }
        //if (dot(outgoing, rec.face_normal) < 0) { outgoing = reflect(outgoing, rec.face_normal); }
        return {outgoing, surface_color * f32(dot(outgoing, rec.hit_normal))};
    }

    virtual VEC_CONSTEXPR RGB emitted(RGB const& base_color, Ray const& ray, TraceRecord const& rec) const noexcept override
    {
        return _emittion;
    }

    virtual VEC_CONSTEXPR RGB operator () (RGB const& base_color, normal3g const& incoming, normal3g const& outgoing, normal3g const& normal) const noexcept override
    {
        // yeah, this BRDF model produces incorrect specular shape.

        RGB reflected_color = lerp(base_color, consts<RGB>::White, _clearcoat);
        f32 pdf = 0;

        normal3g prefect_reflection = -reflect(outgoing, normal);
        vec3g C = fg(1 - _roughness) * prefect_reflection;
        fg r2 = sqr(_roughness);
        fg r_pdf_sphere = 1 / (4 * consts<fg>::pi * _roughness);

        vec3g tmp_v;
        // the probability density of outgoing not being reflected back to the upper hemisphere
        {
            fg t = dot(incoming, C);
            tmp_v = t * incoming - C;
            fg d2 = dot(tmp_v, tmp_v);
            if (d2 < r2)
            {
                fg s = std::sqrt(r2 - d2);
                pdf += r_pdf_sphere * sqr(t + s) / s;
                if (fg tmp = t - s; tmp > 0) { pdf += r_pdf_sphere * sqr(tmp) / s; }
            }
        }
        // the probability density of light being reflected back to the upper hemisphere
        {
            normal3g r_incoming = reflect(incoming, normal);
            fg t = dot(r_incoming, C);
            tmp_v = t * r_incoming - C;
            fg d2 = dot(tmp_v, tmp_v);
            if (d2 < r2)
            {
                fg s = std::sqrt(r2 - d2);
                pdf += r_pdf_sphere * sqr(t + s) / s;
                if (fg tmp = t - s; tmp > 0) { pdf += r_pdf_sphere * sqr(tmp) / s; }
            }
        }

        pdf = lerp(pdf, consts<f32>::inv_pi, _clearcoat);
        return pdf * reflected_color;
    }
};

} // namespace BRDFs
} // namespace nyasRT
