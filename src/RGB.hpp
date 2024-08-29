#pragma once

#include <math.h>

#include "utils.hpp"
#include "geometry/vec3.hpp"


/******** color operations with fp ********/

CONST_FUNC f32 clamp01(f32 x) noexcept
{
    return std::isfinite(x) ? std::min(std::max(x, 0.0f), 1.0f) : 0.0f;
}

CONST_FUNC f32 apply_gamma(f32 x, f32 p) noexcept
{
    return std::pow(x, 1.0f / p);
}
CONST_FUNC f32 apply_gamma(f32 x) noexcept
{
    return apply_gamma(x, 2.2f);
}
CONST_FUNC f32 remove_gamma(f32 x, f32 p) noexcept
{
    return std::pow(x, p);
}
CONST_FUNC f32 remove_gamma(f32 x) noexcept
{
    return remove_gamma(x, 2.2f);
}


class RGB final
{
public:

    f32 r, g, b;

    CONST_FUNC RGB() noexcept
    : r(0.0f), g(0.0f), b(0.0f) {}
    CONST_FUNC RGB(f32 gray) noexcept
    : r(gray), g(gray), b(gray) {}
    CONST_FUNC RGB(f32 r_, f32 g_, f32 b_) noexcept
    : r(r_), g(g_), b(b_) {}
    template<class T> CONST_FUNC RGB(vec3<T> const& v) noexcept
    : r(v.x), g(v.y), b(v.z) {}


    /******** inplace color operations ********/

    CONST_FUNC RGB & clamp01() noexcept
    {
        r = ::clamp01(r); g = ::clamp01(g); b = ::clamp01(b);
        return *this;
    }
    CONST_FUNC RGB & max01() noexcept
    {
        r = std::isfinite(r) ? r : 0.0f;
        g = std::isfinite(g) ? g : 0.0f;
        b = std::isfinite(b) ? b : 0.0f;
        if (f32 n = std::max(std::max(r, g), b); n > 1.0f)
        {
            r /= n; g /= n; b /= n;
        }
        return *this;
    }

    CONST_FUNC RGB & apply_gamma(f32 p) noexcept
    {
        r = ::apply_gamma(r, p); g = ::apply_gamma(g, p); b = ::apply_gamma(b, p);
        return *this;
    }
    CONST_FUNC RGB & apply_gamma() noexcept
    {
        r = ::apply_gamma(r); g = ::apply_gamma(g); b = ::apply_gamma(b);
        return *this;
    }
    CONST_FUNC RGB & remove_gamma(f32 p) noexcept
    {
        r = ::remove_gamma(r, p); g = ::remove_gamma(g, p); b = ::remove_gamma(b, p);
        return *this;
    }
    CONST_FUNC RGB & remove_gamma() noexcept
    {
        r = ::remove_gamma(r); g = ::remove_gamma(g); b = ::remove_gamma(b);
        return *this;
    }


    /******** inplace numerical operations ********/

    CONST_FUNC RGB & operator += (f32 s) noexcept
    {
        r += s; g += s; b += s;
        return *this;
    }
    CONST_FUNC RGB & operator -= (f32 s) noexcept
    {
        r -= s; g -= s; b -= s;
        return *this;
    }
    CONST_FUNC RGB & operator *= (f32 s) noexcept
    {
        r *= s; g *= s; b *= s;
        return *this;
    }
    CONST_FUNC RGB & operator /= (f32 s) noexcept
    {
        r /= s; g /= s; b /= s;
        return *this;
    }

    CONST_FUNC RGB & operator += (RGB const& c) noexcept
    {
        r += c.r; g += c.g; b += c.b;
        return *this;
    }
    CONST_FUNC RGB & operator -= (RGB const& c) noexcept
    {
        r -= c.r; g -= c.g; b -= c.b;
        return *this;
    }
    CONST_FUNC RGB & operator *= (RGB const& c) noexcept
    {
        r *= c.r; g *= c.g; b *= c.b;
        return *this;
    }
    CONST_FUNC RGB & operator /= (RGB const& c) noexcept
    {
        r /= c.r; g /= c.g; b /= c.b;
        return *this;
    }

    template<class T> CONST_FUNC RGB & add(T const& other) noexcept
    {
        (*this) += other;
        return *this;
    }
    template<class T> CONST_FUNC RGB & sub(T const& other) noexcept
    {
        (*this) -= other;
        return *this;
    }
    template<class T> CONST_FUNC RGB & mul(T const& other) noexcept
    {
        (*this) *= other;
        return *this;
    }
    template<class T> CONST_FUNC RGB & div(T const& other) noexcept
    {
        (*this) /= other;
        return *this;
    }
};


/******** default values ********/

template<> class defaults<RGB>
{
public:

    static constexpr RGB Black = RGB(0.0f, 0.0f, 0.0f);
    static constexpr RGB Red   = RGB(1.0f, 0.0f, 0.0f);
    static constexpr RGB Green = RGB(0.0f, 1.0f, 0.0f);
    static constexpr RGB Blue  = RGB(0.0f, 0.0f, 1.0f);
    static constexpr RGB White = RGB(1.0f, 1.0f, 1.0f);
};


/******** color operations ********/

CONST_FUNC RGB clamp01(RGB const& c) noexcept
{
    return RGB(clamp01(c.r), clamp01(c.g), clamp01(c.b));
}
CONST_FUNC RGB max01(RGB const& c) noexcept
{
    RGB tmp(
        isfinite(c.r) ? c.r : 0.0f,
        isfinite(c.g) ? c.g : 0.0f,
        isfinite(c.b) ? c.b : 0.0f
    );
    if (f32 n = std::max(std::max(tmp.r, tmp.g), tmp.b); n > 1.0f)
    {
        tmp /= n;
    }
    return tmp;
}

CONST_FUNC RGB apply_gamma(RGB const& c, f32 p) noexcept
{
    return RGB(apply_gamma(c.r, p), apply_gamma(c.g, p), apply_gamma(c.b, p));
}
CONST_FUNC RGB apply_gamma(RGB const& c) noexcept
{
    return RGB(apply_gamma(c.r), apply_gamma(c.g), apply_gamma(c.b));
}
CONST_FUNC RGB remove_gamma(RGB const& c, f32 p) noexcept
{
    return RGB(remove_gamma(c.r, p), remove_gamma(c.g, p), remove_gamma(c.b, p));
}
CONST_FUNC RGB remove_gamma(RGB const& c) noexcept
{
    return RGB(remove_gamma(c.r), remove_gamma(c.g), remove_gamma(c.b));
}

CONST_FUNC f32 brightness(RGB const& c) noexcept
{
    return 0.2126f * c.r + 0.7152f * c.g + 0.0722f * c.b;
}


/******** inplace numerical operations ********/

CONST_FUNC RGB operator + (RGB const& c, f32 s) noexcept
{
    return RGB(c.r + s, c.g + s, c.b + s);
}
CONST_FUNC RGB operator - (RGB const& c, f32 s) noexcept
{
    return RGB(c.r - s, c.g - s, c.b - s);
}
CONST_FUNC RGB operator * (RGB const& c, f32 s) noexcept
{
    return RGB(c.r * s, c.g * s, c.b * s);
}
CONST_FUNC RGB operator / (RGB const& c, f32 s) noexcept
{
    return RGB(c.r / s, c.g / s, c.b / s);
}

CONST_FUNC RGB operator + (f32 s, RGB const& c) noexcept
{
    return RGB(s + c.r, s + c.g, s + c.b);
}
CONST_FUNC RGB operator - (f32 s, RGB const& c) noexcept
{
    return RGB(s - c.r, s - c.g, s - c.b);
}
CONST_FUNC RGB operator * (f32 s, RGB const& c) noexcept
{
    return RGB(s * c.r, s * c.g, s * c.b);
}
CONST_FUNC RGB operator / (f32 s, RGB const& c) noexcept
{
    return RGB(s / c.r, s / c.g, s / c.b);
}

CONST_FUNC RGB operator + (RGB const& c1, RGB const& c2) noexcept
{
    return RGB(c1.r + c2.r, c1.g + c2.g, c1.b + c2.b);
}
CONST_FUNC RGB operator - (RGB const& c1, RGB const& c2) noexcept
{
    return RGB(c1.r - c2.r, c1.g - c2.g, c1.b - c2.b);
}
CONST_FUNC RGB operator * (RGB const& c1, RGB const& c2) noexcept
{
    return RGB(c1.r * c2.r, c1.g * c2.g, c1.b * c2.b);
}
CONST_FUNC RGB operator / (RGB const& c1, RGB const& c2) noexcept
{
    return RGB(c1.r / c2.r, c1.g / c2.g, c1.b / c2.b);
}

CONST_FUNC RGB sqrt(RGB const& c) noexcept
{
    return RGB(std::sqrt(c.r), std::sqrt(c.g), std::sqrt(c.b));
}
CONST_FUNC RGB exp(RGB const& c) noexcept
{
    return RGB(std::exp(c.r), std::exp(c.g), std::exp(c.b));
}


/******** 3x8 bit RGB value ********/

class RGB24
{
public:

    u8 r, g, b;

    CONST_FUNC RGB24() noexcept
    : RGB24(defaults<RGB>::Black) {}
    CONST_FUNC RGB24(u8 r_, u8 g_, u8 b_) noexcept
    : r{r_}, g{g_}, b{b_} {}
    CONST_FUNC RGB24(RGB const& c) noexcept
    : r(clamp01(c.r) * 255.0f + 0.5f), g(clamp01(c.g) * 255.0f + 0.5f), b(clamp01(c.b) * 255.0f + 0.5f) {}

    CONST_FUNC RGB24 & operator = (RGB const& c) noexcept
    {
        r = clamp01(c.r) * 255.0f + 0.5f;
        g = clamp01(c.g) * 255.0f + 0.5f;
        b = clamp01(c.b) * 255.0f + 0.5f;
        return *this;
    }

    CONST_FUNC bool operator == (RGB24 const& c) const noexcept
    {
        return (r == c.r) && (g == c.g) && (b == c.b);
    }
    CONST_FUNC bool operator == (RGB const& c) const noexcept
    {
        return (*this) == RGB24(c);
    }
};
