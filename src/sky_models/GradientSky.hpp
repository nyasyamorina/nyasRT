#pragma once

#include "Sky.hpp"


class GradientSky : public Sky
{
public:

    RGB top_color, bottom_color;

    GradientSky(RGB const& top, RGB const& bottom)
    : top_color{top}, bottom_color{bottom} {}
    virtual ~GradientSky() noexcept = default;


    virtual RGB operator () (vec3g const& NORMALIZED direction) const noexcept override
    {
        f32 x = defaults<fg>::half * (direction.z + 1);
        return x * top_color + (1 - x) * bottom_color;
    }
};
