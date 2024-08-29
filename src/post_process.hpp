#pragma once

#include <math.h>

#include "utils.hpp"
#include "geometry/vec2.hpp"
#include "RGB.hpp"
#include "Figure.hpp"


// a random function used in `auto_exposure()`
CONST_FUNC f32 _53490_smooth(f32 x) noexcept
{
    x = std::min(std::max(x, 0.0f), 1.0f);
    return (2 * x - 3) * x * x + 1;
}

inline Figure & auto_exposure(Figure & fig) noexcept
{
    // 78 / ((K: reflected-light meter calibration) * (q: lens and vignetting attentuation))
    constexpr f32 _constant = 9.6f;  // K = 12.5; q = 0.65

    // `log(averrage_luminance) = (∑ log(brightness(pixel)) * w) / (∑ w)`
    // where `w(x,y)` is weight function

    f32 averrage_luminance = 0, weights = 0;
    f32 tmp1 = 2.0f / fig.width(); vec2<f32> tmp2 = vec2<f32>(fig.size()) / fig.width();
    RGB const* pixel_p = fig.begin();
    for (u32 h = 0; h < fig.height(); h++)
    {
        for (u32 w = 0; w < fig.width(); w++)
        {
            vec2<u32> pixel_index(w, h);
            f32 weight = _53490_smooth(length(tmp1 * pixel_index - tmp2));
            weights += weight;
            f32 luminance = brightness(*(pixel_p++));
            if (luminance > 1)
            {
                averrage_luminance += std::log(luminance) * weight;
            }
        }
    }
    averrage_luminance = std::exp(averrage_luminance / weights);

    f32 exposure = 1 / (_constant * averrage_luminance);
    for (RGB & pixel : fig) { pixel *= exposure; }

    return fig;
}

inline Figure & ACES_tone_mapping(Figure & fig) noexcept
{
    for (RGB & pixel : fig)
    {
        constexpr f32 a = 2.51f, b = 0.03f, c = 2.43f, d = 0.59f, e = 0.14f;
        pixel = ((a * pixel + b) * pixel) / ((c * pixel + d) * pixel + e);
    }
    return fig;
}

inline Figure & output_ready(Figure & fig) noexcept
{
    for (RGB & pixel : fig)
    {
        pixel.clamp01().apply_gamma();
    }
    return fig;
}
