#pragma once

#include <math.h>

#include "../common.hpp"
#include "GraphicsBuffer.hpp"


namespace nyasRT
{
// a random function used in `auto_exposure()`
constexpr inline f32 cubic_smooth(f32 x) noexcept
{
    x = std::min(std::max(x, 0.0f), 1.0f);
    return (2 * x - 3) * x * x + 1;
}

inline GraphicsBuffer & auto_exposure(GraphicsBuffer & gbuf) noexcept
{
    // 78 / ((K: reflected-light meter calibration) * (q: lens and vignetting attentuation))
    constexpr f32 _constant = 9.6f;  // K = 12.5; q = 0.65

    using vec2f32 = glm::vec<2, f32, glm::precision::packed_lowp>;

    // `log(averrage_luminance) = (∑ log(brightness(pixel)) * w) / (∑ w)`
    // where `w(x,y)` is weight function

    f32 averrage_luminance = 0, weights = 0;
    f32 tmp1 = 2.0f / gbuf.width(); vec2f32 tmp2(1, 1 / gbuf.aspect_ratio());
    for (SubBuffer iterator(gbuf); auto & iter : iterator)
    {
        vec2f32 pixel_index = iter.global_index();
        f32 weight = cubic_smooth(length(tmp1 * pixel_index - tmp2));
        weights += weight;
        f32 lum = luminance(iter.pixel());
        if (lum > 1)
        {
            averrage_luminance += std::log(lum) * weight;
        }
    }
    averrage_luminance = std::exp(averrage_luminance / weights);

    f32 exposure = 1 / (_constant * averrage_luminance);
    for (RGB & pixel : gbuf) { pixel *= exposure; }

    return gbuf;
}

inline GraphicsBuffer & ACES_tone_mapping(GraphicsBuffer & fig) noexcept
{
    for (RGB & pixel : fig)
    {
        constexpr f32 a = 2.51f, b = 0.03f, c = 2.43f, d = 0.59f, e = 0.14f;
        pixel = ((a * pixel + b) * pixel) / ((c * pixel + d) * pixel + e);
    }
    return fig;
}

inline GraphicsBuffer & output_ready(GraphicsBuffer & fig) noexcept
{
    for (RGB & pixel : fig)
    {
        pixel = apply_gamma(clamp01(pixel));
    }
    return fig;
}

} // namespace nyasRT
