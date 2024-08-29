#pragma once

#include "Texture.hpp"


class PureColor : public Texture
{
public:

    RGB color;

    PureColor()
    : color{0.8f} {}
    PureColor(RGB const& color_)
    : color(color_) {}
    virtual ~PureColor() noexcept = default;

    virtual RGB operator () (Ray const& ray, TraceRecord const& rec) const noexcept override
    {
        return color;
    }
};