#pragma once

#include "Sky.hpp"


namespace nyasRT
{
namespace sky_models
{
class GradientSky : public Sky
{
public:

    RGB top_color, bottom_color;

    VEC_CONSTEXPR GradientSky(RGB const& top, RGB const& bottom)
    : top_color{top}, bottom_color{bottom} {}
    virtual ~GradientSky() noexcept = default;


    virtual VEC_CONSTEXPR RGB operator () (normal3g const& direction) const noexcept override
    {
        f32 x = consts<fg>::half * (direction.z + 1);
        return lerp(bottom_color, top_color, x);
    }
};

} // namespace sky_models
} // namespace nyasRT
