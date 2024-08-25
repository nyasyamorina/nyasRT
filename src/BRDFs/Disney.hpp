#pragma once

#include <math.h>

#include "BRDF.hpp"


class DisneyBRDF final : public BRDF
{
public:

    static CONST_FUNC f32 schlick_fresnel(f32 x) noexcept
    {
        return pow5(1.0f - x);
    }
    static CONST_FUNC f32 D_gtr1(f32 h_dot_n, f32 a) noexcept
    {
        f32 aa = sqr(a);
        f32 t = 1.0f + (aa - 1.0f) * sqr(h_dot_n);
        return (aa - 1.0f) / (defaults<f32>::two_pi * std::log(a) * t);
    }
    static CONST_FUNC f32 D_gtr2(f32 h_dot_n, f32 a) noexcept
    {
        f32 aa = sqr(a);
        f32 t = 1.0f + (aa - 1.0f) * sqr(h_dot_n);
        return aa / (defaults<f32>::pi * sqr(t));
    }
    static CONST_FUNC f32 smith_ggx(f32 v_dot_n, f32 a) noexcept
    {
        f32 aa = sqr(a);
        f32 vv = sqr(v_dot_n);
        return 1.0f / (v_dot_n + std::sqrt(aa + vv - aa * vv));
    }

    class Diffuse : public BRDF
    {
    public:

        RGB base_color;
        f32 roughness;

        CONST_FUNC Diffuse(RGB const& base_color_, f32 roughness_)
        : base_color{base_color_}, roughness{roughness_} {}
        virtual ~Diffuse() noexcept = default;

        virtual CONST_FUNC RGB operator () (vec3g const& NORMALIZED incoming, vec3g const& NORMALIZED outgoing, vec3g const& NORMALIZED normal) const noexcept override
        {
            f32 i_dot_n = dot(incoming, normal), o_dot_n = dot(outgoing, normal);
            if ((i_dot_n < 0) || (o_dot_n < 0)) { return defaults<RGB>::Black; }
            vec3g half_vec = normalize(incoming + outgoing);
            f32 d = dot(incoming, half_vec);

            f32 f_d90 = defaults<f32>::half + 2 * roughness * sqr(d);
            f32 fresnel_i = 1 + (f_d90 - 1) * schlick_fresnel(i_dot_n);
            f32 fresnel_o = 1 + (f_d90 - 1) * schlick_fresnel(o_dot_n);
            f32 fd = defaults<f32>::inv_pi * fresnel_i * fresnel_o;
            return base_color * fd;
        }
    };

private:

    RGB _base_color;
    f32 _subsurface, _metalic, _specular, _specular_tint, _roughness, _sheen, _sheen_tint, _clearcoat, _clearcoat_gloss;

public:

    CONST_FUNC DisneyBRDF() noexcept
    : _base_color{defaults<RGB>::White}, _subsurface{0.5f}, _metalic{0.5f}, _specular{0.5f}, _specular_tint{0.5f}, _roughness{0.5f}
    , _sheen{0.5f}, _sheen_tint{0.5f}, _clearcoat{0.5f}, _clearcoat_gloss{0.5f} {}
    virtual ~DisneyBRDF() noexcept = default;

    CONST_FUNC DisneyBRDF & base_color(RGB const& value) noexcept
    {
        _base_color = value;
        return *this;
    }
    CONST_FUNC DisneyBRDF & subsurface(f32 value) noexcept
    {
        _subsurface = value;
        return *this;
    }
    CONST_FUNC DisneyBRDF & metalic(f32 value) noexcept
    {
        _metalic = value;
        return *this;
    }
    CONST_FUNC DisneyBRDF & specular(f32 value) noexcept
    {
        _specular = value;
        return *this;
    }
    CONST_FUNC DisneyBRDF & specular_tint(f32 value) noexcept
    {
        _specular_tint = value;
        return *this;
    }
    CONST_FUNC DisneyBRDF & roughness(f32 value) noexcept
    {
        _roughness = std::max(defaults<f32>::eps, value);
        return *this;
    }
    CONST_FUNC DisneyBRDF & sheen(f32 value) noexcept
    {
        _sheen = value;
        return *this;
    }
    CONST_FUNC DisneyBRDF & sheen_tint(f32 value) noexcept
    {
        _sheen_tint = value;
        return *this;
    }
    CONST_FUNC DisneyBRDF & clearcoat(f32 value) noexcept
    {
        _clearcoat = value;
        return *this;
    }
    CONST_FUNC DisneyBRDF & clearcoat_gloss(f32 value) noexcept
    {
        _clearcoat_gloss = value;
        return *this;
    }

    CONST_FUNC RGB const& base_color() const noexcept
    {
        return _base_color;
    }
    CONST_FUNC f32 subsurface() const noexcept
    {
        return _subsurface;
    }
    CONST_FUNC f32 metalic() const noexcept
    {
        return _metalic;
    }
    CONST_FUNC f32 specular() const noexcept
    {
        return _specular;
    }
    CONST_FUNC f32 specular_tint() const noexcept
    {
        return _specular_tint;
    }
    CONST_FUNC f32 roughness() const noexcept
    {
        return _roughness;
    }
    CONST_FUNC f32 sheen() const noexcept
    {
        return _sheen;
    }
    CONST_FUNC f32 sheen_tint() const noexcept
    {
        return _sheen_tint;
    }
    CONST_FUNC f32 clearcoat() const noexcept
    {
        return _clearcoat;
    }
    CONST_FUNC f32 clearcoat_gloss() const noexcept
    {
        return _clearcoat_gloss;
    }


    // https://github.com/wdas/brdf/blob/main/src/brdfs/disney.brdf
    virtual CONST_FUNC RGB operator () (vec3g const& NORMALIZED incoming, vec3g const& NORMALIZED outgoing, vec3g const& NORMALIZED normal) const noexcept override
    {
        f32 i_dot_n = dot(incoming, normal), o_dot_n = dot(outgoing, normal);
        if ((i_dot_n < 0) || (o_dot_n < 0)) { return defaults<RGB>::Black; }
        vec3g half_vec = normalize(incoming + outgoing);
        f32 i_dot_h = dot(incoming, half_vec), h_dot_n = dot(half_vec, normal);

        f32 lum = brightness(_base_color);
        RGB tint_color = _base_color / lum;
        RGB specular_color = mix(_specular * 0.08f * mix(1.0f, tint_color, _specular_tint), _base_color, _metalic);
        RGB sheen_color = mix(1.0f, tint_color, _sheen_tint);

        f32 fi = schlick_fresnel(i_dot_n), fo = schlick_fresnel(o_dot_n), fh = schlick_fresnel(i_dot_h);

        // diffuse
        f32 fss90 = _roughness * sqr(i_dot_h);
        f32 fd90 = 0.5f + 2.0f * fss90;
        f32 fd  = mix(1.0f, fd90,  fi) * mix(1.0f, fd90,  fo);
        f32 fss = mix(1.0f, fss90, fi) * mix(1.0f, fss90, fo);
        f32 ss = 1.25f * (fss * (1.0f / (i_dot_n + o_dot_n) - 0.5f) + 0.5f);

        // specular; TODO: _anisotropic
        f32 alpha = sqr(_roughness);
        f32 ds = D_gtr2(h_dot_n, alpha);
        RGB fs = mix(specular_color, 1.0f, fh);
        f32 gs = smith_ggx(i_dot_n, alpha) * smith_ggx(o_dot_n, alpha);

        // sheen
        RGB fsheen = fh * _sheen * sheen_color;

        // clearcoat
        f32 dr = D_gtr1(h_dot_n, mix(0.1f, 0.001f, _clearcoat_gloss));
        f32 fr = mix(0.04f, 1.0f, fh);
        f32 gr = smith_ggx(i_dot_n, 0.25f) * smith_ggx(o_dot_n, 0.25f);

        return (defaults<f32>::inv_pi * mix(fd, ss, _subsurface) * _base_color + fsheen) * (1.0f - _metalic)
            + gs * ds * fs
            + 0.25f * _clearcoat * gr * fr * dr;
    }
};
