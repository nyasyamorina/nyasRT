#pragma once

#include "Material.hpp"


namespace nyasRT
{
namespace materials
{
class PureColor : public Material
{
public:

    RGB color;

    VEC_CONSTEXPR PureColor()
    : color{0.8f} {}
    VEC_CONSTEXPR PureColor(RGB const& color_)
    : color(color_) {}
    virtual ~PureColor() noexcept = default;

    virtual VEC_CONSTEXPR RGB operator () (Ray const& ray, TraceRecord const& rec) const noexcept override
    {
        return color;
    }
};

} // namespace materials
} // namespace nyasRT
