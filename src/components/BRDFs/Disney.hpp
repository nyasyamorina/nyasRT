#pragma once

#include <math.h>

#include "BRDF.hpp"



namespace nyasRT
{
namespace BRDFs
{
class DisneyBRDF final : public BRDF
{
private:

    static constexpr inline f32 schlick_fresnel(f32 x) noexcept
    {
        return pow5(1.0f - x);
    }
    static constexpr inline f32 D_gtr1(f32 h_dot_n, f32 a) noexcept
    {
        f32 aa = sqr(a);
        f32 t = 1.0f + (aa - 1.0f) * sqr(h_dot_n);
        return (aa - 1.0f) / (consts<f32>::two_pi * std::log(a) * t);
    }
    static constexpr inline f32 D_gtr2(f32 h_dot_n, f32 a) noexcept
    {
        f32 aa = sqr(a);
        f32 t = 1.0f + (aa - 1.0f) * sqr(h_dot_n);
        return aa / (consts<f32>::pi * sqr(t));
    }
    static constexpr inline f32 smith_ggx(f32 v_dot_n, f32 a) noexcept
    {
        f32 aa = sqr(a);
        f32 vv = sqr(v_dot_n);
        return 1.0f / (v_dot_n + std::sqrt(aa + vv - aa * vv));
    }


    f32 _subsurface, _metalic, _specular, _specular_tint, _roughness, _sheen, _sheen_tint, _clearcoat, _clearcoat_gloss;

public:

    constexpr DisneyBRDF() noexcept
    : _subsurface{0.5f}, _metalic{0.5f}, _specular{0.5f}, _specular_tint{0.5f}, _roughness{0.5f}
    , _sheen{0.5f}, _sheen_tint{0.5f}, _clearcoat{0.5f}, _clearcoat_gloss{0.5f} {}
    virtual ~DisneyBRDF() noexcept = default;

    constexpr DisneyBRDF & subsurface(f32 value) noexcept
    {
        _subsurface = value;
        return *this;
    }
    constexpr DisneyBRDF & metalic(f32 value) noexcept
    {
        _metalic = value;
        return *this;
    }
    constexpr DisneyBRDF & specular(f32 value) noexcept
    {
        _specular = value;
        return *this;
    }
    constexpr DisneyBRDF & specular_tint(f32 value) noexcept
    {
        _specular_tint = value;
        return *this;
    }
    constexpr DisneyBRDF & roughness(f32 value) noexcept
    {
        _roughness = std::max(consts<f32>::eps, value);
        return *this;
    }
    constexpr DisneyBRDF & sheen(f32 value) noexcept
    {
        _sheen = value;
        return *this;
    }
    constexpr DisneyBRDF & sheen_tint(f32 value) noexcept
    {
        _sheen_tint = value;
        return *this;
    }
    constexpr DisneyBRDF & clearcoat(f32 value) noexcept
    {
        _clearcoat = value;
        return *this;
    }
    constexpr DisneyBRDF & clearcoat_gloss(f32 value) noexcept
    {
        _clearcoat_gloss = value;
        return *this;
    }

    constexpr f32 subsurface() const noexcept
    {
        return _subsurface;
    }
    constexpr f32 metalic() const noexcept
    {
        return _metalic;
    }
    constexpr f32 specular() const noexcept
    {
        return _specular;
    }
    constexpr f32 specular_tint() const noexcept
    {
        return _specular_tint;
    }
    constexpr f32 roughness() const noexcept
    {
        return _roughness;
    }
    constexpr f32 sheen() const noexcept
    {
        return _sheen;
    }
    constexpr f32 sheen_tint() const noexcept
    {
        return _sheen_tint;
    }
    constexpr f32 clearcoat() const noexcept
    {
        return _clearcoat;
    }
    constexpr f32 clearcoat_gloss() const noexcept
    {
        return _clearcoat_gloss;
    }


    // https://github.com/wdas/brdf/blob/main/src/brdfs/disney.brdf
    virtual VEC_CONSTEXPR RGB operator () (RGB const& base_color, normal3g const& incoming, normal3g const& outgoing, normal3g const& normal) const noexcept override
    {
        f32 i_dot_n = dot(incoming, normal), o_dot_n = dot(outgoing, normal);
        if ((i_dot_n < 0) || (o_dot_n < 0)) { return consts<RGB>::Black; }
        vec3g half_vec = normalize(incoming + outgoing);
        f32 i_dot_h = dot(incoming, half_vec), h_dot_n = dot(half_vec, normal);

        f32 inv_lum = 1 / luminance(base_color);
        RGB tint_color = base_color * inv_lum;
        RGB specular_color = lerp(consts<RGB>::White, tint_color, _specular_tint);
        specular_color = lerp(0.08f * _specular * specular_color, base_color, _metalic);
        RGB sheen_color = lerp(consts<RGB>::White, tint_color, _sheen_tint);

        f32 fi = schlick_fresnel(i_dot_n), fo = schlick_fresnel(o_dot_n), fh = schlick_fresnel(i_dot_h);

        // diffuse
        f32 fss90 = _roughness * sqr(i_dot_h);
        f32 fd90 = 0.5f + 2.0f * fss90;
        f32 fd  = lerp(1.0f, fd90,  fi) * lerp(1.0f, fd90,  fo);
        f32 fss = lerp(1.0f, fss90, fi) * lerp(1.0f, fss90, fo);
        f32 ss = 1.25f * (fss * (1.0f / (i_dot_n + o_dot_n) - 0.5f) + 0.5f);

        // specular; TODO: _anisotropic
        f32 alpha = sqr(_roughness);
        f32 ds = D_gtr2(h_dot_n, alpha);
        RGB fs = lerp(specular_color, consts<RGB>::White, fh);
        f32 gs = smith_ggx(i_dot_n, alpha) * smith_ggx(o_dot_n, alpha);

        // sheen
        RGB fsheen = fh * _sheen * sheen_color;

        // clearcoat
        f32 dr = D_gtr1(h_dot_n, lerp(0.1f, 0.001f, _clearcoat_gloss));
        f32 fr = lerp(0.04f, 1.0f, fh);
        f32 gr = smith_ggx(i_dot_n, 0.25f) * smith_ggx(o_dot_n, 0.25f);

        return (consts<f32>::inv_pi * lerp(fd, ss, _subsurface) * base_color + fsheen) * (1.0f - _metalic)
            + gs * ds * fs
            + 0.25f * _clearcoat * gr * fr * dr;
    }
};

} // namespace BRDFs
} // namespace nyasRT
