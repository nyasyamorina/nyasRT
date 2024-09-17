#pragma once

#include <chrono>
#include <functional>
#include <inttypes.h>
#include <math.h>
#include <string>
#include <type_traits>

#include "../third-party/glm/glm.hpp"

// TODO: follow up https://github.com/g-truc/glm/pull/1313
#define VEC_CONSTEXPR GLM_CONSTEXPR
#if (GLM_HAS_CONSTEXPR)
#   define VEC_CONST constexpr
#else
#   define VEC_CONST const
#endif


namespace nyasRT
{
/********** basic types **********/

namespace basic_types
{
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

#if defined(NYASRT_USE_DOUBLE_PRECISION_GEOMETRY)
// the floating-point number used in geometry calculations
using fg = f64;
#else
// the floating-point number used in geometry calculations
using fg = f32;
#endif

using size2_t  = glm::vec<2, u32>;
using index2_t = glm::vec<2, i32>;

using RGB   = glm::vec<3, f32, glm::precision::packed_lowp>;
using RGB24 = glm::vec<3, u8>;

using vec2g = glm::vec<2, fg, glm::precision::packed_highp>;
using vec3g = glm::vec<3, fg, glm::precision::packed_highp>;
using vec4g = glm::vec<4, fg, glm::precision::packed_highp>;

// indicates this vector should be normalized
using normal3g = vec3g;

} // namespace basic_types
using namespace basic_types;


/********** constant values **********/

template<class T> struct consts;

template<> struct consts<f32>
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
template<> struct consts<f64>
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

template<> struct consts<RGB>
{
public:
#if (GLM_HAS_CONSTEXPR)
    static constexpr RGB Black = RGB(0, 0, 0);
    static constexpr RGB Red   = RGB(1, 0, 0);
    static constexpr RGB Green = RGB(0, 1, 0);
    static constexpr RGB Blue  = RGB(0, 0, 1);
    static constexpr RGB White = RGB(1, 1, 1);
#else
    static RGB const Black;
    static RGB const Red;
    static RGB const Green;
    static RGB const Blue;
    static RGB const White;
#endif
};

template<> struct consts<vec2g>
{
public:
#if (GLM_HAS_CONSTEXPR)
    static constexpr vec2g O = vec2g(0, 0);
    static constexpr vec2g X = vec2g(1, 0);
    static constexpr vec2g Y = vec2g(0, 1);
    static constexpr vec2g min = -vec2g(consts<fg>::inf, consts<fg>::inf);
    static constexpr vec2g max =  vec2g(consts<fg>::inf, consts<fg>::inf);
#else
    static vec2g const O;
    static vec2g const X;
    static vec2g const Y;
    static vec2g const min;
    static vec2g const max;
#endif
};
template<> struct consts<vec3g>
{
public:
#if (GLM_HAS_CONSTEXPR)
    static constexpr vec3g O = vec3g(0, 0, 0);
    static constexpr vec3g X = vec3g(1, 0, 0);
    static constexpr vec3g Y = vec3g(0, 1, 0);
    static constexpr vec3g Z = vec3g(0, 0, 1);
    static constexpr vec3g min = -vec3g(consts<fg>::inf, consts<fg>::inf, consts<fg>::inf);
    static constexpr vec3g max =  vec3g(consts<fg>::inf, consts<fg>::inf, consts<fg>::inf);
#else
    static vec3g const O;
    static vec3g const X;
    static vec3g const Y;
    static vec3g const Z;
    static vec3g const min;
    static vec3g const max;
#endif
};
template<> struct consts<vec4g>
{
public:
#if (GLM_HAS_CONSTEXPR)
    static constexpr vec4g O = vec4g(0, 0, 0, 0);
    static constexpr vec4g X = vec4g(1, 0, 0, 0);
    static constexpr vec4g Y = vec4g(0, 1, 0, 0);
    static constexpr vec4g Z = vec4g(0, 0, 1, 0);
    static constexpr vec4g W = vec4g(0, 0, 0, 1);
    static constexpr vec4g min = -vec4g(consts<fg>::inf, consts<fg>::inf, consts<fg>::inf, consts<fg>::inf);
    static constexpr vec4g max =  vec4g(consts<fg>::inf, consts<fg>::inf, consts<fg>::inf, consts<fg>::inf);
#else
    static vec4g const O;
    static vec4g const X;
    static vec4g const Y;
    static vec4g const Z;
    static vec4g const W;
    static vec4g const min;
    static vec4g const max;
#endif
};

#if (!GLM_HAS_CONSTEXPR)
RGB const consts<RGB>::Black = RGB(0, 0, 0);
RGB const consts<RGB>::Red   = RGB(1, 0, 0);
RGB const consts<RGB>::Green = RGB(0, 1, 0);
RGB const consts<RGB>::Blue  = RGB(0, 0, 1);
RGB const consts<RGB>::White = RGB(1, 1, 1);

vec2g const consts<vec2g>::O = vec2g(0, 0);
vec2g const consts<vec2g>::X = vec2g(1, 0);
vec2g const consts<vec2g>::Y = vec2g(0, 1);
vec2g const consts<vec2g>::min = -vec2g(consts<fg>::inf, consts<fg>::inf);
vec2g const consts<vec2g>::max =  vec2g(consts<fg>::inf, consts<fg>::inf);

vec3g const consts<vec3g>::O = vec3g(0, 0, 0);
vec3g const consts<vec3g>::X = vec3g(1, 0, 0);
vec3g const consts<vec3g>::Y = vec3g(0, 1, 0);
vec3g const consts<vec3g>::Z = vec3g(0, 0, 1);
vec3g const consts<vec3g>::min = -vec3g(consts<fg>::inf, consts<fg>::inf, consts<fg>::inf);
vec3g const consts<vec3g>::max =  vec3g(consts<fg>::inf, consts<fg>::inf, consts<fg>::inf);

vec4g const consts<vec4g>::O = vec4g(0, 0, 0, 0);
vec4g const consts<vec4g>::X = vec4g(1, 0, 0, 0);
vec4g const consts<vec4g>::Y = vec4g(0, 1, 0, 0);
vec4g const consts<vec4g>::Z = vec4g(0, 0, 1, 0);
vec4g const consts<vec4g>::W = vec4g(0, 0, 0, 1);
vec4g const consts<vec4g>::min = -vec4g(consts<fg>::inf, consts<fg>::inf, consts<fg>::inf, consts<fg>::inf);
vec4g const consts<vec4g>::max =  vec4g(consts<fg>::inf, consts<fg>::inf, consts<fg>::inf, consts<fg>::inf);
#endif


/********** math functions **********/

template<class T> constexpr inline auto rad2deg(T rad) noexcept
{
    using FP = std::common_type_t<T, f32>;
    constexpr FP a = 180 / consts<FP>::pi;
    return a * rad;
}
template<class T> constexpr inline auto deg2rad(T deg) noexcept
{
    using FP = std::common_type_t<T, f32>;
    constexpr FP a = consts<FP>::pi / 180;
    return a * deg;
}

template<class X, class Y> constexpr inline auto clamp(X const& x, Y const& min_, Y const& max_)
{
    using ::std::min, ::std::max;
    using ::glm::min, ::glm::max;
    return min(max(x, min_), max_);
}

// linear interpolation
template<class X, class T> constexpr inline auto lerp(X const& x0, X const& x1, T const& t) noexcept
{
    return x0 + (x1 - x0) * t;
}
template<class F> constexpr inline  F _cerp_coef2(F const& t) noexcept
{
    return static_cast<F>(1.0 / 6.0) * (t * t - 1) * t;
}
template<class F> constexpr inline  F _cerp_coef1(F const& t) noexcept
{
    return ((1 - t) * t * static_cast<F>(1.0 / 2.0) + 1) * t;
}
// cubic interpolation
template<class X, class T> constexpr inline auto cerp(X const& x_1, X const& x0, X const& x1, X const& x2, T const& t) noexcept
{
    T c_1 = _cerp_coef2(1 - t);
    T c0  = _cerp_coef1(1 - t);
    T c1  = _cerp_coef1(t);
    T c2  = _cerp_coef2(t);
    return c_1 * x_1 + c0 * x0 + c1 * x1 + c2 * x2;
}


/********** power functions **********/

template<class T> constexpr inline T sqr(T const& x) noexcept
{
    return x * x;
}
template<class T> constexpr inline T cub(T const& x) noexcept
{
    return x * x * x;
}
template<class T> constexpr inline T pow4(T const& x) noexcept
{
    return sqr(sqr(x));
}
template<class T> constexpr inline T pow5(T const& x) noexcept
{
    return sqr(sqr(x)) * x;
}


/********** color functions **********/

template<bool check_inf = true> constexpr inline f32 clamp01(f32 x) noexcept
{
    using std::min, std::max;
    if constexpr (check_inf)
    {
        // `std::isfinite` is not constexpr in MSVC, not even in C++23
        x = std::isfinite(x) ? x : 0.0f;
    }
    return min(max(x, 0.0f), 1.0f);
}
template<bool check_inf = true> VEC_CONSTEXPR inline RGB clamp01(RGB const& c) noexcept
{
    return RGB(clamp01<check_inf>(c.r), clamp01<check_inf>(c.g), clamp01<check_inf>(c.b));
}

constexpr inline f32 remove_gamma(f32 x) noexcept
{
    using ::std::pow;
    return pow(x, 2.2f);
}
VEC_CONSTEXPR inline RGB remove_gamma(RGB const& c) noexcept
{
    using ::glm::pow;
    return pow(c, RGB(2.2f));
}

constexpr inline f32 apply_gamma(f32 x) noexcept
{
    using ::std::pow;
    return pow(x, 1.0f / 2.2f);
}
VEC_CONSTEXPR inline RGB apply_gamma(RGB const& c) noexcept
{
    using ::glm::pow;
    return pow(c, RGB(1.0f / 2.2f));
}

constexpr inline f32 luminance(RGB const& c) noexcept
{
    return 0.2126f * c.r + 0.7152f * c.g + 0.0722f * c.b;
}

template<bool check_inf = true> VEC_CONSTEXPR inline RGB24 RGB_to_RGB24(RGB c) noexcept
{
    c = clamp01<check_inf>(c) * 255.0f + 0.5f;
    return RGB24(c);
}
VEC_CONSTEXPR inline RGB RGB24_to_RGB(RGB24 c) noexcept
{
    return RGB(c) * (1.0f / 255.0f);
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


/********** timer **********/

inline f32 timeit(std::function<void()> const& func) noexcept
{
    using namespace std::chrono;

    auto time_start = system_clock::now();
    func();
    auto time_stop = system_clock::now();

    f32 milis = duration_cast<milliseconds>(time_stop - time_start).count();
    return milis / 1000;
}

} // namespace nyasRT
