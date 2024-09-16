#pragma once

#include <math.h>

#include "Material.hpp"


namespace nyasRT
{
namespace materials
{
class FunctionalMaterial : public Material
{
public:

    RGB color1, color2;

    VEC_CONSTEXPR FunctionalMaterial(RGB const& color1_, RGB const& color2_)
    : color1{color1_}, color2{color2_} {}
    virtual ~FunctionalMaterial() noexcept = default;

    virtual VEC_CONSTEXPR RGB operator () (Ray const& ray, TraceRecord const& rec) const noexcept override
    {
        constexpr f32 a = 0.976f, k = 0.288449914061f, s = 0.647957546708f;

        f32 w = rec.hit_point.x + rec.hit_point.y + rec.hit_point.z;
        w = s * (std::cbrt(std::sin(12 * w) + a) + k);
        return lerp(color2, color1, w);
    }
};

} // namespace materials
} // namespace nyasRT
