#pragma once

#include <array>
#include <math.h>

#include "../utils.hpp"
#include "Sky.hpp"
#include "ArHosekSkyModelData_RGB.h"


constexpr f32 solar_radius = deg2rad(0.255f);

class ArHosek : public Sky
{
protected:

    vec3g _solar_direcion;
    f32 _turbidity;
    RGB _albedo;

    RGB _configs[9];
    RGB _radiance;
    f32 _emission_correction_factor_sky[3];
    f32 _emission_correction_factor_sun[3];

public:

    CONST_FUNC ArHosek() noexcept
    : _solar_direcion{normalize(defaults<vec3g>::X + defaults<vec3g>::Z)}, _turbidity{5}, _albedo(defaults<RGB>::White) {}
    virtual ~ArHosek() noexcept = default;

    CONST_FUNC ArHosek & solar_direcion(vec3g const& value) noexcept
    {
        _solar_direcion = normalize(value);
        return *this;
    }
    CONST_FUNC ArHosek & turbidity(f32 value) noexcept
    {
        constexpr f32 max_turbidity = 11 * (1 - defaults<fg>::eps);
        _turbidity = std::min(std::max(value, 0.0f), max_turbidity);
        return *this;
    }
    CONST_FUNC ArHosek & albedo(RGB const& value) noexcept
    {
        _albedo = value;
        return *this;
    }

    CONST_FUNC vec3g const& solar_direcion() const noexcept
    {
        return _solar_direcion;
    }
    CONST_FUNC f32 turbidity() const noexcept
    {
        return _turbidity;
    }
    CONST_FUNC RGB const& albedo() const noexcept
    {
        return _albedo;
    }


    virtual bool prepare() noexcept override
    {
        i32 t_index = std::floor(_turbidity);
        f32 interpolate = _turbidity - t_index;
        f32 tmp_a = std::pow(std::acos(_solar_direcion.z) / defaults<f32>::half_pi, defaults<f32>::third), tmp_b = 1 - tmp_a;

        f32 a2 = sqr(tmp_a), a3 = a2 * tmp_a, a4 = a3 * tmp_a, a5 = a4 * tmp_a;
        f32 b2 = sqr(tmp_b), b3 = b2 * tmp_b, b4 = b3 * tmp_b, b5 = b4 * tmp_b;
        f32 a1b4 = 5 * tmp_a * b4, a2b3 = 10 * a2 * b3, a3b2 = 10 * a3 * b2, a4b1 = 5 * a4 * tmp_b;
        RGB c0, c1, c2, c3, c4, c5;

        // alb 0 low turb

        i32 rm_index = 6 * (t_index - 1), m_index = 9 * rm_index;
        f64 * rdata = datasetsRGB[0] + m_index;      f64 * gdata = datasetsRGB[1] + m_index;      f64 * bdata = datasetsRGB[2] + m_index;
        f64 * rrdata = datasetsRGBRad[0] + rm_index; f64 * grdata = datasetsRGBRad[1] + rm_index; f64 * brdata = datasetsRGBRad[2] + rm_index;

        for (u8 k = 0; k < 9; k++)
        {
            c0 = RGB(rdata[k     ], gdata[k     ], bdata[k     ]);
            c1 = RGB(rdata[k +  9], gdata[k +  9], bdata[k +  9]);
            c2 = RGB(rdata[k + 18], gdata[k + 18], bdata[k + 18]);
            c3 = RGB(rdata[k + 27], gdata[k + 27], bdata[k + 27]);
            c4 = RGB(rdata[k + 36], gdata[k + 36], bdata[k + 36]);
            c5 = RGB(rdata[k + 45], gdata[k + 45], bdata[k + 45]);
            _configs[k] = (1 - _albedo) * (1 - interpolate) * (
                b5 * c0 + a1b4 * c1 + a2b3 * c2 + a3b2 * c3 + a4b1 * c4 + a5 * c5
            );
        }
        c0 = RGB(rrdata[0], grdata[0], brdata[0]);
        c1 = RGB(rrdata[1], grdata[1], brdata[1]);
        c2 = RGB(rrdata[2], grdata[2], brdata[2]);
        c3 = RGB(rrdata[3], grdata[3], brdata[3]);
        c4 = RGB(rrdata[4], grdata[4], brdata[4]);
        c5 = RGB(rrdata[5], grdata[5], brdata[5]);
        _radiance = (1 - _albedo) * (1 - interpolate) * (
            b5 * c0 + a1b4 * c1 + a2b3 * c2 + a3b2 * c3 + a4b1 * c4 + a5 * c5
        );

        // alb 1 low turb

        rdata += 9 * 6 * 10; gdata += 9 * 6 * 10; bdata += 9 * 6 * 10;
        rrdata += 6 * 10;    grdata += 6 * 10;    brdata += 6 * 10;

        for (u8 k = 0; k < 9; k++)
        {
            c0 = RGB(rdata[k     ], gdata[k     ], bdata[k     ]);
            c1 = RGB(rdata[k +  9], gdata[k +  9], bdata[k +  9]);
            c2 = RGB(rdata[k + 18], gdata[k + 18], bdata[k + 18]);
            c3 = RGB(rdata[k + 27], gdata[k + 27], bdata[k + 27]);
            c4 = RGB(rdata[k + 36], gdata[k + 36], bdata[k + 36]);
            c5 = RGB(rdata[k + 45], gdata[k + 45], bdata[k + 45]);
            _configs[k] += _albedo * (1 - interpolate) * (
                b5 * c0 + a1b4 * c1 + a2b3 * c2 + a3b2 * c3 + a4b1 * c4 + a5 * c5
            );
        }
        c0 = RGB(rrdata[0], grdata[0], brdata[0]);
        c1 = RGB(rrdata[1], grdata[1], brdata[1]);
        c2 = RGB(rrdata[2], grdata[2], brdata[2]);
        c3 = RGB(rrdata[3], grdata[3], brdata[3]);
        c4 = RGB(rrdata[4], grdata[4], brdata[4]);
        c5 = RGB(rrdata[5], grdata[5], brdata[5]);
        _radiance += _albedo * (1 - interpolate) * (
            b5 * c0 + a1b4 * c1 + a2b3 * c2 + a3b2 * c3 + a4b1 * c4 + a5 * c5
        );

        if (t_index == 10) { return true; }

        // alb 0 high turb

        rm_index = 6 * t_index; m_index = 9 * rm_index;
        rdata = datasetsRGB[0] + m_index;      gdata = datasetsRGB[1] + m_index;      bdata = datasetsRGB[2] + m_index;
        rrdata = datasetsRGBRad[0] + rm_index; grdata = datasetsRGBRad[1] + rm_index; brdata = datasetsRGBRad[2] + rm_index;

        for (u8 k = 0; k < 9; k++)
        {
            c0 = RGB(rdata[k     ], gdata[k     ], bdata[k     ]);
            c1 = RGB(rdata[k +  9], gdata[k +  9], bdata[k +  9]);
            c2 = RGB(rdata[k + 18], gdata[k + 18], bdata[k + 18]);
            c3 = RGB(rdata[k + 27], gdata[k + 27], bdata[k + 27]);
            c4 = RGB(rdata[k + 36], gdata[k + 36], bdata[k + 36]);
            c5 = RGB(rdata[k + 45], gdata[k + 45], bdata[k + 45]);
            _configs[k] += (1 - _albedo) * interpolate * (
                b5 * c0 + a1b4 * c1 + a2b3 * c2 + a3b2 * c3 + a4b1 * c4 + a5 * c5
            );
        }
        c0 = RGB(rrdata[0], grdata[0], brdata[0]);
        c1 = RGB(rrdata[1], grdata[1], brdata[1]);
        c2 = RGB(rrdata[2], grdata[2], brdata[2]);
        c3 = RGB(rrdata[3], grdata[3], brdata[3]);
        c4 = RGB(rrdata[4], grdata[4], brdata[4]);
        c5 = RGB(rrdata[5], grdata[5], brdata[5]);
        _radiance += (1 - _albedo) * interpolate * (
            b5 * c0 + a1b4 * c1 + a2b3 * c2 + a3b2 * c3 + a4b1 * c4 + a5 * c5
        );

        // alb 1 high turb

        rdata += 9 * 6 * 10; gdata += 9 * 6 * 10; bdata += 9 * 6 * 10;
        rrdata += 6 * 10;    grdata += 6 * 10;    brdata += 6 * 10;

        for (u8 k = 0; k < 9; k++)
        {
            c0 = RGB(rdata[k     ], gdata[k     ], bdata[k     ]);
            c1 = RGB(rdata[k +  9], gdata[k +  9], bdata[k +  9]);
            c2 = RGB(rdata[k + 18], gdata[k + 18], bdata[k + 18]);
            c3 = RGB(rdata[k + 27], gdata[k + 27], bdata[k + 27]);
            c4 = RGB(rdata[k + 36], gdata[k + 36], bdata[k + 36]);
            c5 = RGB(rdata[k + 45], gdata[k + 45], bdata[k + 45]);
            _configs[k] += _albedo * interpolate * (
                b5 * c0 + a1b4 * c1 + a2b3 * c2 + a3b2 * c3 + a4b1 * c4 + a5 * c5
            );
        }
        c0 = RGB(rrdata[0], grdata[0], brdata[0]);
        c1 = RGB(rrdata[1], grdata[1], brdata[1]);
        c2 = RGB(rrdata[2], grdata[2], brdata[2]);
        c3 = RGB(rrdata[3], grdata[3], brdata[3]);
        c4 = RGB(rrdata[4], grdata[4], brdata[4]);
        c5 = RGB(rrdata[5], grdata[5], brdata[5]);
        _radiance += _albedo * interpolate * (
            b5 * c0 + a1b4 * c1 + a2b3 * c2 + a3b2 * c3 + a4b1 * c4 + a5 * c5
        );

        return true;
    }


    virtual RGB operator () (vec3g const& NORMALIZED direction) const noexcept override
    {
        if (direction.z < 0) { return defaults<RGB>::Black; }
        f32 cgamma = dot(_solar_direcion, direction);

        RGB expM = exp(_configs[4] * std::acos(cgamma));
        f32 rayM = sqr(cgamma);
        RGB mieM = (1 + rayM) / sqrt(cub(1 + sqr(_configs[8]) - 2 * cgamma * _configs[8]));
        f32 zenith = std::sqrt(direction.z);

        RGB tmp1 = 1 + _configs[0] + exp(_configs[1] / (direction.z + 0.01));
        RGB tmp2 = _configs[2] + _configs[3] * expM + _configs[5] * rayM + _configs[6] * mieM + _configs[7] * zenith;
        return _radiance * tmp1 * tmp2;
    }
};
