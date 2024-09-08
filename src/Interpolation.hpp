#pragma once

#include <math.h>

#include "utils.hpp"
#include "geometry/vec2.hpp"
#include "RGB.hpp"


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
    static CONST_FUNC RGB nearest(RGB const* data, vec2<u32> size, vec2g pos) noexcept
    {
        pos.x = std::fmod(pos.x * size.x, size.x); pos.y = std::fmod(pos.y * size.y, size.y);
        vec2<u32> index(std::floor(pos.x), std::floor(pos.y));

        return data[index.x + index.y * size.x];
    }
    /// @param pos in [0,1)^2
    static CONST_FUNC RGB bilinear(RGB const* data, vec2<u32> size, vec2g pos) noexcept
    {
        pos.x = std::fmod(pos.x * size.x, size.x); pos.y = std::fmod(pos.y * size.y, size.y);
        vec2<u32> index0(std::floor(pos.x), std::floor(pos.y));
        pos -= index0;
        vec2<u32> index1 = (index0 + 1) % size;

        RGB tmp0 = lerp(data[index0.x + index0.y * size.x], data[index1.x + index0.y * size.x], pos.x);
        RGB tmp1 = lerp(data[index0.x + index1.y * size.x], data[index1.x + index1.y * size.x], pos.x);
        return lerp(tmp0, tmp1, pos.y);
    }
    /// @param pos in [0,1)^2
    static CONST_FUNC RGB bicubic(RGB const* data, vec2<u32> size, vec2g pos) noexcept
    {
        pos.x = std::fmod(pos.x * size.x, size.x); pos.y = std::fmod(pos.y * size.y, size.y);
        vec2<u32> index0(std::floor(pos.x), std::floor(pos.y));
        pos -= index0;
        vec2<u32> index1 = (index0 + 1) % size;
        vec2<u32> index_1 = (index0 + (size - 1)) % size;
        vec2<u32> index2 = (index0 + 2) % size;

        RGB tmp_1 = cerp(data[index_1.x + index_1.y * size.x], data[index0.x + index_1.y * size.x], data[index1.x + index_1.y * size.x], data[index2.x + index_1.y * size.x], pos.x);
        RGB tmp0  = cerp(data[index_1.x + index0.y  * size.x], data[index0.x + index0.y  * size.x], data[index1.x + index0.y  * size.x], data[index2.x + index0.y  * size.x], pos.x);
        RGB tmp1  = cerp(data[index_1.x + index1.y  * size.x], data[index0.x + index1.y  * size.x], data[index1.x + index1.y  * size.x], data[index2.x + index1.y  * size.x], pos.x);
        RGB tmp2  = cerp(data[index_1.x + index2.y  * size.x], data[index0.x + index2.y  * size.x], data[index1.x + index2.y  * size.x], data[index2.x + index2.y  * size.x], pos.x);
        return cerp(tmp_1, tmp0, tmp1, tmp2, pos.y);
    }

private:

    Method _method;

public:

    Interpolation() noexcept = default;
    CONST_FUNC Interpolation(Method method) noexcept
    : _method{method} {}

    CONST_FUNC operator Method() const noexcept
    {
        return _method;
    }

    /// @param pos in [0,1)^2
    CONST_FUNC RGB operator () (RGB const* data, vec2<u32> size, vec2g pos) const noexcept
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
