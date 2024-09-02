#pragma once

#include <array>
#include <math.h>
#include <memory>

//#define HOSEK_USING_CIEXYZ

#include "../utils.hpp"
#include "../light_sources/SunLight.hpp"
#include "Sky.hpp"
#ifdef HOSEK_USING_CIEXYZ
#include "ArHosekSkyModelData_CIEXYZ.h"
#else
#include "ArHosekSkyModelData_RGB.h"
#endif


// there is an un-found bug in this implement
class Hosek : public Sky
{
protected:

    class QuinticBezier
    {
    public:

        f32 coefs[6];

        CONST_FUNC QuinticBezier(f32 t) noexcept
        {
            f32 t2 = t  * t;
            f32 t3 = t2 * t;
            f32 t4 = t3 * t;
            f32 t5 = t4 * t;

            f32 v = 1 - t;
            f32 v2 = v  * v;
            f32 v3 = v2 * v;
            f32 v4 = v3 * v;
            f32 v5 = v4 * v;

            coefs[0] = v5          ;
            coefs[1] = v4 * t  *  5;
            coefs[2] = v3 * t2 * 10;
            coefs[3] = v2 * t3 * 10;
            coefs[4] = v  * t4 *  5;
            coefs[5]=       t5     ;
        }

        template<class T> CONST_FUNC T operator () (T const* ps) const noexcept
        {
            return coefs[0] * ps[0] + coefs[1] * ps[1] + coefs[2] * ps[2] + coefs[3] * ps[3] + coefs[4] * ps[4] + coefs[5] * ps[5];
        }
    };

    static inline RGB * _get_coefs(RGB * ps, u32 albedo, u32 turbidity, u32 coef_index) noexcept
    {
#ifdef HOSEK_USING_CIEXYZ
        f64 ** dataset = datasetsXYZ;
#else
        f64 ** dataset = datasetsRGB;
#endif
        f64 * r_dataset = dataset[0];
        f64 * g_dataset = dataset[1];
        f64 * b_dataset = dataset[2];

        u32 index = (((albedo * 10) + (turbidity - 1)) * 6 /* + k */) * 9 + coef_index;

        for (u32 k = 0; k < 6; k++)
        {
            ps[k].r = r_dataset[index + k * 9];
            ps[k].g = g_dataset[index + k * 9];
            ps[k].b = b_dataset[index + k * 9];
        }
        return ps;
    }
    static inline RGB * _get_radiance(RGB * ps, u32 albedo, u32 turbidity) noexcept
    {
#ifdef HOSEK_USING_CIEXYZ
        f64 ** dataset = datasetsXYZRad;
#else
        f64 ** dataset = datasetsRGBRad;
#endif
        f64 * r_dataset = dataset[0];
        f64 * g_dataset = dataset[1];
        f64 * b_dataset = dataset[2];

        u32 index = ((albedo * 10) + (turbidity - 1)) * 6 /* + k */;

        for (u32 k = 0; k < 6; k++)
        {
            ps[k].r = r_dataset[index + k];
            ps[k].g = g_dataset[index + k];
            ps[k].b = b_dataset[index + k];
        }
        return ps;
    }


    vec3g _solar_direcion;
    f32 _turbidity;
    RGB _albedo;

    RGB _coefs[9];
    RGB _radiance;

public:

    CONST_FUNC Hosek() noexcept
    : _solar_direcion{normalize(defaults<vec3g>::X + defaults<vec3g>::Z)}, _turbidity{5}, _albedo(defaults<RGB>::White) {}
    virtual ~Hosek() noexcept = default;

    CONST_FUNC Hosek & solar_direcion(normal3g const& value) noexcept
    {
        _solar_direcion = normalize(value);
        return *this;
    }
    CONST_FUNC Hosek & turbidity(f32 value) noexcept
    {
        _turbidity = std::min(std::max(value, 1.0f), 10.0f);
        return *this;
    }
    CONST_FUNC Hosek & albedo(RGB const& value) noexcept
    {
#ifdef HOSEK_USING_CIEXYZ
        _albedo = defaults<RGB>::Rxyz * value.r + defaults<RGB>::Gxyz * value.g + defaults<RGB>::Bxyz * value.b;
#else
        _albedo = value;
#endif
        return *this;
    }

    CONST_FUNC normal3g const& solar_direcion() const noexcept
    {
        return _solar_direcion;
    }
    CONST_FUNC f32 turbidity() const noexcept
    {
        return _turbidity;
    }
    CONST_FUNC RGB albedo() const noexcept
    {
#ifdef HOSEK_USING_CIEXYZ
        return defaults<RGB>::Xrgb * _albedo.r + defaults<RGB>::Yrgb * _albedo.g + defaults<RGB>::Zrgb * _albedo.b;
#else
        return _albedo;
#endif
    }

    std::shared_ptr<SunLight> sun() const noexcept
    {
        return std::make_shared<SunLight>(_solar_direcion, _turbidity);
    }


    virtual CONST_FUNC bool prepare() noexcept override
    {
        i32 t_index = std::floor(_turbidity);
        f32 t_rem = _turbidity - t_index;
        f32 solar_elevation = defaults<f32>::half_pi - std::acos(_solar_direcion.z);

        RGB ps[6];
        QuinticBezier interpolate(std::cbrt(solar_elevation / defaults<f32>::half_pi));

        // alb 0 low turb
        RGB weight = (1 - _albedo) * (1 - t_rem);
        _radiance = weight * interpolate(_get_radiance(ps, 0, t_index));
        for (u8 k = 0; k < 9; k++)
        {
            _coefs[k] = weight * interpolate(_get_coefs(ps, 0, t_index, k));
        }

        // alb 1 low turb
        weight = _albedo * (1 - t_rem);
        _radiance += weight * interpolate(_get_radiance(ps, 1, t_index));
        for (u8 k = 0; k < 9; k++)
        {
            _coefs[k] += weight * interpolate(_get_coefs(ps, 1, t_index, k));
        }

        if (t_index == 10) { return true; }

        // alb 0 high turb
        weight = (1 - _albedo) * t_rem;
        _radiance += weight * interpolate(_get_radiance(ps, 0, t_index + 1));
        for (u8 k = 0; k < 9; k++)
        {
            _coefs[k] += weight * interpolate(_get_coefs(ps, 0, t_index + 1, k));
        }

        // alb 1 high turb
        weight = _albedo * t_rem;
        _radiance += weight * interpolate(_get_radiance(ps, 1, t_index + 1));
        for (u8 k = 0; k < 9; k++)
        {
            _coefs[k] += weight * interpolate(_get_coefs(ps, 1, t_index + 1, k));
        }

        return true;
    }


    virtual CONST_FUNC RGB operator () (normal3g const& direction) const noexcept override
    {
        f32 ctheta = std::max(static_cast<fg>(0), direction.z);
        f32 cgamma = dot(_solar_direcion, direction);

        RGB expM = exp(_coefs[4] * std::acos(cgamma));
        f32 rayM = sqr(cgamma);
        RGB mieM = (1 + rayM) / sqrt(cub(1 + sqr(_coefs[8]) - 2 * cgamma * _coefs[8]));
        f32 zenith = std::sqrt(ctheta);

        RGB sky_color = _radiance;
        sky_color *= 1 + _coefs[0] * exp(_coefs[1] / (ctheta + 0.01f));
        sky_color *= _coefs[2] + _coefs[3] * expM + _coefs[5] * rayM + _coefs[6] * mieM + _coefs[7] * zenith;

#ifdef HOSEK_USING_CIEXYZ
        return defaults<RGB>::Xrgb * sky_color.r + defaults<RGB>::Yrgb * sky_color.g + defaults<RGB>::Zrgb * sky_color.b;
#else
        return sky_color;
#endif
    }
};
