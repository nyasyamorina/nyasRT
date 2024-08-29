#pragma once

#include <math.h>

#include "Texture.hpp"


class FunctionalTexture : public Texture
{
public:

    RGB color1, color2;

    FunctionalTexture(RGB const& color1_, RGB const& color2_)
    : color1{color1_}, color2{color2_} {}
    virtual ~FunctionalTexture() noexcept = default;

    virtual RGB operator () (Ray const& ray, TraceRecord const& rec) const noexcept override
    {
        constexpr f32 a = 0.976f, k = 0.288449914061f, s = 0.647957546708f;

        f32 w = sum(rec.hit_point);
        w = s * (std::cbrt(std::sin(12 * w) + a) + k);
        return mix(color2, color1, w);
    }
};
