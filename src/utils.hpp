#pragma once

#include <inttypes.h>
#include <math.h>
#include <numeric>
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


/********** numerical functions **********/

// linear interpolation
template<class X, class T> CONST_FUNC auto lerp(X const& x0, X const& x1, T const& t) noexcept
{
    return x0 + (x1 - x0) * t;
}
template<class F> CONST_FUNC F _cerp_coef2(F const& t) noexcept
{
    return static_cast<F>(1.0 / 6.0) * (t * t - 1) * t;
}
template<class F> CONST_FUNC F _cerp_coef1(F const& t) noexcept
{
    return ((1 - t) * t * static_cast<F>(1.0 / 2.0) + 1) * t;
}
// cubic interpolation
template<class X, class T> CONST_FUNC auto cerp(X const& x_1, X const& x0, X const& x1, X const& x2, T const& t) noexcept
{
    T c_1 = _cerp_coef2(1 - t);
    T c0  = _cerp_coef1(1 - t);
    T c1  = _cerp_coef1(t);
    T c2  = _cerp_coef2(t);
    return c_1 * x_1 + c0 * x0 + c1 * x1 + c2 * x2;
}

template<class T> CONST_FUNC T sqr(T const& x) noexcept
{
    return x * x;
}
template<class T> CONST_FUNC T cub(T const& x) noexcept
{
    return x * x * x;
}
template<class T> CONST_FUNC T pow4(T const& x) noexcept
{
    return sqr(sqr(x));
}
template<class T> CONST_FUNC T pow5(T const& x) noexcept
{
    return sqr(sqr(x)) * x;
}


/********** default values **********/

template<class T> class defaults;

template<> class defaults<f32>
{
public:

    static constexpr f32 half    = 0.5f;
    static constexpr f32 third   = 1.0f / 3.0f;
    static constexpr f32 pi      = 3.1415927f;
    static constexpr f32 two_pi  = 2.0f * pi;
    static constexpr f32 inv_pi  = 1.0f / pi;
    static constexpr f32 half_pi = half * pi;
    static constexpr f32 eps     = 1e-5f;
    static constexpr f32 inf     = std::numeric_limits<f32>::max();
};
template<> class defaults<f64>
{
public:

    static constexpr f64 half    = 0.5;
    static constexpr f64 third   = 1.0 / 3.0;
    static constexpr f64 pi      = 3.141592653589793;
    static constexpr f64 two_pi  = 2.0 * pi;
    static constexpr f64 inv_pi  = 1.0 / pi;
    static constexpr f64 half_pi = half * pi;
    static constexpr f64 eps     = 1e-8;
    static constexpr f64 inf     = std::numeric_limits<f64>::max();
};


/********** degree <--> radian **********/

template<class FP = f64> CONST_FUNC FP rad2deg(FP rad) noexcept
{
    constexpr FP a = 180 / defaults<FP>::pi;
    return a * rad;
}
template<class FP = f64> CONST_FUNC FP deg2rad(FP deg) noexcept
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
