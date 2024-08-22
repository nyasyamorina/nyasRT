#pragma once

#include <inttypes.h>
#include <math.h>
#include <string>
#include <type_traits>


#define CONST_FUNC constexpr inline


using i8  = int8_t;
using i16 = int16_t;
using i32 = int32_t;
using i64 = int64_t;

using u8  = uint8_t;
using u16 = uint16_t;
using u32 = uint32_t;
using u64 = uint64_t;

using f32 = float;
using f64 = double;


// the floating-point number used in geometry calculations
using fg = f32;


/********** default values **********/

template<class T> class defaults;

template<> class defaults<f32>
{
public:

    static constexpr f32 pi  = 3.1415927f;
    static constexpr f32 eps = 1e-5f;
};
template<> class defaults<f64>
{
public:

    static constexpr f64 pi  = 3.141592653589793;
    static constexpr f64 eps = 1e-8;
};


/********** degree <--> radian **********/

template<class FP> CONST_FUNC FP rad2deg(FP rad) noexcept
{
    constexpr FP a = 180 / defaults<FP>::pi;
    return a * rad;
}
template<class FP> CONST_FUNC FP deg2rad(FP deg) noexcept
{
    constexpr FP a = defaults<FP>::pi / 180;
    return a * deg;
}


/********** convert string to number **********/

template<class T> inline T s2n(std::string const&);

template<> inline i32 s2n<i32>(std::string const& str)
{
    return std::stoi(str);
}
template<> inline i64 s2n<i64>(std::string const& str)
{
    return std::stoll(str);
}

template<> inline u32 s2n<u32>(std::string const& str)
{
    return std::stoul(str);
}
template<> inline u64 s2n<u64>(std::string const& str)
{
    return std::stoull(str);
}

template<> inline f32 s2n<f32>(std::string const& str)
{
    return std::stof(str);
}
template<> inline f64 s2n<f64>(std::string const& str)
{
    return std::stod(str);
}
