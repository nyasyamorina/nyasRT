#pragma once

#include <math.h>

#include "../common.hpp"
#include "GraphicsBuffer.hpp"


namespace nyasRT
{
class Interpolation final
{
public:

    enum Method : u8
    {
        Nearest,
        Bilinear,
        Bicubic
    };

    /// @param pos in [0,1)^2
    static VEC_CONSTEXPR inline RGB nearest(RGB const* data, size2_t size, vec2g pos) noexcept
    {
        pos.x = std::fmod(pos.x * size.x, size.x); pos.y = std::fmod(pos.y * size.y, size.y);
        index2_t const index(std::floor(pos.x), std::floor(pos.y));

        return data[index.x + index.y * size.x];
    }
    /// @param pos in [0,1)^2
    static VEC_CONSTEXPR inline RGB bilinear(RGB const* data, size2_t size, vec2g pos) noexcept
    {
        pos.x = std::fmod(pos.x * size.x, size.x); pos.y = std::fmod(pos.y * size.y, size.y);
        index2_t const index0(std::floor(pos.x), std::floor(pos.y));
        pos -= index0;
        index2_t const index1 = (index0 + 1) % index2_t(size);

        RGB tmp0 = lerp(data[index0.x + index0.y * size.x], data[index1.x + index0.y * size.x], f32(pos.x));
        RGB tmp1 = lerp(data[index0.x + index1.y * size.x], data[index1.x + index1.y * size.x], f32(pos.x));
        return lerp(tmp0, tmp1, f32(pos.y));
    }
    /// @param pos in [0,1)^2
    static VEC_CONSTEXPR inline RGB bicubic(RGB const* data, size2_t size, vec2g pos) noexcept
    {
        index2_t const isize = size;
        pos.x = std::fmod(pos.x * size.x, size.x); pos.y = std::fmod(pos.y * size.y, size.y);
        index2_t const index0(std::floor(pos.x), std::floor(pos.y));
        pos -= index0;
        index2_t const index1 = (index0 + 1) % isize;
        index2_t const index_1 = (index0 + (isize - 1)) % isize;
        index2_t const index2 = (index0 + 2) % isize;

        RGB tmp_1 = cerp(data[index_1.x + index_1.y * size.x], data[index0.x + index_1.y * size.x], data[index1.x + index_1.y * size.x], data[index2.x + index_1.y * size.x], f32(pos.x));
        RGB tmp0  = cerp(data[index_1.x + index0.y  * size.x], data[index0.x + index0.y  * size.x], data[index1.x + index0.y  * size.x], data[index2.x + index0.y  * size.x], f32(pos.x));
        RGB tmp1  = cerp(data[index_1.x + index1.y  * size.x], data[index0.x + index1.y  * size.x], data[index1.x + index1.y  * size.x], data[index2.x + index1.y  * size.x], f32(pos.x));
        RGB tmp2  = cerp(data[index_1.x + index2.y  * size.x], data[index0.x + index2.y  * size.x], data[index1.x + index2.y  * size.x], data[index2.x + index2.y  * size.x], f32(pos.x));
        return cerp(tmp_1, tmp0, tmp1, tmp2, f32(pos.y));
    }

private:

    Method _method;

public:

    constexpr Interpolation() noexcept = default;
    constexpr Interpolation(Method method) noexcept
    : _method{method} {}

    constexpr operator Method() const noexcept
    {
        return _method;
    }

    /// @param pos in [0,1)^2
    VEC_CONSTEXPR RGB operator()(GraphicsBuffer const& gbuf, vec2g pos) const noexcept
    {
        return operator()(gbuf.data(), gbuf.size(), pos);
    }
    /// @param pos in [0,1)^2
    VEC_CONSTEXPR RGB operator()(RGB const* data, size2_t size, vec2g pos) const noexcept
    {
        switch (_method)
        {
        case Bilinear:
            return bilinear(data, size, pos);
        case Bicubic:
            return bicubic(data, size, pos);
        default:
            return nearest(data, size, pos);
        };
    }
};

} // namespace nyasRT
