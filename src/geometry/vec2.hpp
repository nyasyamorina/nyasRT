#pragma once

#include <functional>
#include <math.h>
#include <numeric>
#include <tuple>
#include <type_traits>

#include "../utils.hpp"


template<class T> class vec2 final
{
    static_assert(std::is_arithmetic_v<T> || std::is_same_v<T, bool>, "element type of vector must be number or boolean");

public:

    using eltype = T;

    T x, y;

    CONST_FUNC vec2() noexcept
    {}
    CONST_FUNC vec2(T const& s) noexcept
    : x(s), y(s) {}
    explicit CONST_FUNC vec2(T const& x_, T const& y_) noexcept
    : x(x_), y(y_) {}
    CONST_FUNC vec2(vec2 const&) = default;

    template<class U> CONST_FUNC vec2(vec2<U> const& v) noexcept
    : x(v.x), y(v.y) {}
    template<class U> CONST_FUNC vec2 & operator = (vec2<U> const& v) noexcept
    {
        x = v.x; y = v.y;
        return *this;
    }


    /******** map to each elements ********/

    CONST_FUNC vec2 & apply(std::function<void(T &)> const& func) noexcept
    {
        func(x); func(y);
        return *this;
    }

    template<class U> CONST_FUNC vec2<U> map(std::function<U(T &)> const& func) noexcept
    {
        return vec2(func(x), func(y));
    }
    template<class U> CONST_FUNC vec2<U> map(std::function<U(T const&)> const& func) const noexcept
    {
        return vec2(func(x), func(y));
    }


    /******** indexing ********/

    CONST_FUNC vec2<T> xx() const noexcept
    {
        return vec2(x, x);
    }
    CONST_FUNC vec2<T> yx() const noexcept
    {
        return vec2(y, x);
    }
    CONST_FUNC vec2<T> xy() const noexcept
    {
        return vec2(x, y);
    }
    CONST_FUNC vec2<T> yy() const noexcept
    {
        return vec2(y, y);
    }


    /******** inplace geometry operations ********/

    CONST_FUNC vec2 & normalize() noexcept
    {
        static_assert(std::is_floating_point_v<T>, "`normalize` must done in fp vectors.");
        T n = std::sqrt(x * x + y * y);
        (*this) /= n;
        return *this;
    }
    // normalize vector and return this length before normalized.
    CONST_FUNC T normlen() noexcept
    {
        static_assert(std::is_floating_point_v<T>, "`normlen` must done in fp vectors.");
        T n = std::sqrt(x * x + y * y);
        (*this) /= n;
        return n;
    }

    template<class U> CONST_FUNC vec2 & reflect(vec2<U> const& NORMALIZED n) noexcept
    {
        auto a = x * n.x + y * n.y;
        constexpr decltype(a) two = static_cast<decltype(a)>(2);
        x -= two * a * n.x; y -= two * a * n.y;
        return *this;
    }

    /******** inplace numerical operations ********/

    CONST_FUNC vec2 & neg() noexcept
    {
        if constexpr (std::is_same_v<T, bool>)
        {
            x = !x; y = !y;
        }
        else
        {
            x = -x; y = -y;
        }
        return *this;
    }

    template<class U> CONST_FUNC vec2 & operator += (U const& s) noexcept
    {
        x += s; y += s;
        return *this;
    }
    template<class U> CONST_FUNC vec2 & operator -= (U const& s) noexcept
    {
        x -= s; y -= s;
        return *this;
    }
    template<class U> CONST_FUNC vec2 & operator *= (U const& s) noexcept
    {
        x *= s; y *= s;
        return *this;
    }
    template<class U> CONST_FUNC vec2 & operator /= (U const& s) noexcept
    {
        x /= s; y /= s;
        return *this;
    }
    template<class U> CONST_FUNC vec2 & operator %= (U const& s) noexcept
    {
        x %= s; y %= s;
        return *this;
    }
    template<class U> CONST_FUNC vec2 & operator |= (U const& s) noexcept
    {
        x |= s; y |= s;
        return *this;
    }
    template<class U> CONST_FUNC vec2 & operator &= (U const& s) noexcept
    {
        x &= s; y &= s;
        return *this;
    }
    template<class U> CONST_FUNC vec2 & operator ^= (U const& s) noexcept
    {
        x ^= s; y ^= s;
        return *this;
    }
    template<class U> CONST_FUNC vec2 & operator <<=(U const& s) noexcept
    {
        x <<= s; y <<= s;
        return *this;
    }
    template<class U> CONST_FUNC vec2 & operator >>=(U const& s) noexcept
    {
        x >>= s; y >>= s;
        return *this;
    }

    template<class U> CONST_FUNC vec2 & operator += (vec2<U> const& v) noexcept
    {
        x += v.x; y += v.y;
        return *this;
    }
    template<class U> CONST_FUNC vec2 & operator -= (vec2<U> const& v) noexcept
    {
        x -= v.x; y -= v.y;
        return *this;
    }
    template<class U> CONST_FUNC vec2 & operator *= (vec2<U> const& v) noexcept
    {
        x *= v.x; y *= v.y;
        return *this;
    }
    template<class U> CONST_FUNC vec2 & operator /= (vec2<U> const& v) noexcept
    {
        x /= v.x; y /= v.y;
        return *this;
    }
    template<class U> CONST_FUNC vec2 & operator %= (vec2<U> const& v) noexcept
    {
        x %= v.x; y %= v.y;
        return *this;
    }
    template<class U> CONST_FUNC vec2 & operator |= (vec2<U> const& v) noexcept
    {
        x |= v.x; y |= v.y;
        return *this;
    }
    template<class U> CONST_FUNC vec2 & operator &= (vec2<U> const& v) noexcept
    {
        x &= v.x; y &= v.y;
        return *this;
    }
    template<class U> CONST_FUNC vec2 & operator ^= (vec2<U> const& v) noexcept
    {
        x ^= v.x; y ^= v.y;
        return *this;
    }
    template<class U> CONST_FUNC vec2 & operator <<=(vec2<U> const& v) noexcept
    {
        x <<= v.x; y <<= v.y;
        return *this;
    }
    template<class U> CONST_FUNC vec2 & operator >>=(vec2<U> const& v) noexcept
    {
        x >>= v.x; y >>= v.y;
        return *this;
    }

    template<class U> CONST_FUNC vec2 & add(U const& other) noexcept
    {
        (*this) += other;
        return *this;
    }
    template<class U> CONST_FUNC vec2 & sub(U const& other) noexcept
    {
        (*this) -= other;
        return *this;
    }
    template<class U> CONST_FUNC vec2 & mul(U const& other) noexcept
    {
        (*this) *= other;
        return *this;
    }
    template<class U> CONST_FUNC vec2 & div(U const& other) noexcept
    {
        (*this) /= other;
        return *this;
    }
    template<class U> CONST_FUNC vec2 & mod(U const& other) noexcept
    {
        (*this) &= other;
        return *this;
    }

    /******** compare each elements operations ********/

    template <class U> CONST_FUNC vec2<bool> isless(U const& s) const noexcept
    {
        return vec2(x < s, y < s);
    }
    template <class U> CONST_FUNC vec2<bool> isleq(U const& s) const noexcept
    {
        return vec2(x <= s, y <= s);
    }
    template <class U> CONST_FUNC vec2<bool> iseq(U const& s) const noexcept
    {
        return vec2(x == s, y == s);
    }
    template <class U> CONST_FUNC vec2<bool> isneq(U const& s) const noexcept
    {
        return vec2(x != s, y != s);
    }
    template <class U> CONST_FUNC vec2<bool> isgeq(U const& s) const noexcept
    {
        return vec2(x >= s, y >= s);
    }
    template <class U> CONST_FUNC vec2<bool> isgreater(U const& s) const noexcept
    {
        return vec2(x > s, y > s);
    }

    template <class U> CONST_FUNC vec2<bool> isless(vec2<U> const& v) const noexcept
    {
        return vec2(x < v.x, y < v.y);
    }
    template <class U> CONST_FUNC vec2<bool> isleq(vec2<U> const& v) const noexcept
    {
        return vec2(x <= v.x, y <= v.y);
    }
    template <class U> CONST_FUNC vec2<bool> iseq(vec2<U> const& v) const noexcept
    {
        return vec2(x == v.x, y == v.y);
    }
    template <class U> CONST_FUNC vec2<bool> isneq(vec2<U> const& v) const noexcept
    {
        return vec2(x != v.x, y != v.y);
    }
    template <class U> CONST_FUNC vec2<bool> isgeq(vec2<U> const& v) const noexcept
    {
        return vec2(x >= v.x, y >= v.y);
    }
    template <class U> CONST_FUNC vec2<bool> isgreater(vec2<U> const& v) const noexcept
    {
        return vec2(x > v.x, y > v.y);
    }
};

// the vector type used in geometry calculations
using vec2g = vec2<fg>;


/********** default values **********/

template<class T> class defaults<vec2<T>>
{
public:

    static constexpr vec2<T> O = vec2<T>(static_cast<T>(0), static_cast<T>(0));
    static constexpr vec2<T> X = vec2<T>(static_cast<T>(1), static_cast<T>(0));
    static constexpr vec2<T> Y = vec2<T>(static_cast<T>(0), static_cast<T>(1));
    static constexpr vec2<T> min = vec2<T>(std::numeric_limits<T>::min());
    static constexpr vec2<T> max = vec2<T>(std::numeric_limits<T>::max());
};


/********** geometry operations **********/

template<class T, class U> CONST_FUNC std::common_type_t<T, U> dot(vec2<T> const& v1, vec2<U> const& v2) noexcept
{
    return v1.x * v2.x + v1.y * v2.y;
}

template<class T> CONST_FUNC T length2(vec2<T> const& v) noexcept
{
    return dot(v, v);
}
template<class T> CONST_FUNC T length(vec2<T> const& v) noexcept
{
    static_assert(std::is_floating_point_v<T>, "`length` must done in fp vectors.");
    return std::sqrt(length2(v));
}

template<class T> CONST_FUNC vec2<T> normalize(vec2<T> const& v) noexcept
{
    static_assert(std::is_floating_point_v<T>, "`normalize` must done in fp vectors.");
    T n = length(v);
    return vec2(v.x / n, v.y / n);
}
// normalized vector and the length of input vector
template<class T> CONST_FUNC std::tuple<vec2<T>, T> normlen(vec2<T> const& v) noexcept
{
    static_assert(std::is_floating_point_v<T>, "`normlen` must done in fp vectors.");
    T n = length(v);
    return {vec2(v.x / n, v.y / n), n};
}

template<class T, class U> vec2<std::common_type_t<T, U>> reflect(vec2<T> const& v, vec2<U> const& NORMALIZED n) noexcept
{
    return v - static_cast<std::common_type_t<T, U>>(2) * dot(v, n) * n;
}


/******** numerical operations ********/

template<class T> CONST_FUNC T sum(vec2<T> const& v) noexcept
{
    return v.x + v.y;
}
template<class T> CONST_FUNC T prod(vec2<T> const& v) noexcept
{
    return v.x * v.y;
}

template<class T> CONST_FUNC vec2<T> operator + (vec2<T> const& v) noexcept
{
    return vec2(+v.x, +v.y);
}
template<class T> CONST_FUNC vec2<T> operator - (vec2<T> const& v) noexcept
{
    return vec2(-v.x, -v.y);
}
template<class T> CONST_FUNC vec2<T> operator ~ (vec2<T> const& v) noexcept
{
    return vec2(~v.x, ~v.y);
}
CONST_FUNC vec2<bool> operator ! (vec2<bool> const& v) noexcept
{
    return vec2(!v.x, !v.y);
}

CONST_FUNC bool all(vec2<bool> const& v) noexcept
{
    return v.x && v.y;
}
CONST_FUNC bool any(vec2<bool> const& v) noexcept
{
    return v.x || v.y;
}
CONST_FUNC bool not_all_equal(vec2<bool> const& v) noexcept
{
    return (!v.x) != (!v.y);
}

template<class T, class U> CONST_FUNC vec2<std::common_type_t<T, U>> operator + (vec2<T> const& v, U const& s) noexcept
{
    return vec2(v.x + s, v.y + s);
}
template<class T, class U> CONST_FUNC vec2<std::common_type_t<T, U>> operator - (vec2<T> const& v, U const& s) noexcept
{
    return vec2(v.x - s, v.y - s);
}
template<class T, class U> CONST_FUNC vec2<std::common_type_t<T, U>> operator * (vec2<T> const& v, U const& s) noexcept
{
    return vec2(v.x * s, v.y * s);
}
template<class T, class U> CONST_FUNC vec2<std::common_type_t<T, U>> operator / (vec2<T> const& v, U const& s) noexcept
{
    return vec2(v.x / s, v.y / s);
}
template<class T, class U> CONST_FUNC vec2<std::common_type_t<T, U>> operator % (vec2<T> const& v, U const& s) noexcept
{
    return vec2(v.x % s, v.y % s);
}
template<class T, class U> CONST_FUNC vec2<std::common_type_t<T, U>> operator | (vec2<T> const& v, U const& s) noexcept
{
    return vec2(v.x | s, v.y | s);
}
template<class T, class U> CONST_FUNC vec2<std::common_type_t<T, U>> operator & (vec2<T> const& v, U const& s) noexcept
{
    return vec2(v.x & s, v.y & s);
}
template<class T, class U> CONST_FUNC vec2<std::common_type_t<T, U>> operator ^ (vec2<T> const& v, U const& s) noexcept
{
    return vec2(v.x ^ s, v.y ^ s);
}
template<class T, class U> CONST_FUNC vec2<std::common_type_t<T, U>> operator <<(vec2<T> const& v, U const& s) noexcept
{
    return vec2(v.x << s, v.y << s);
}
template<class T, class U> CONST_FUNC vec2<std::common_type_t<T, U>> operator >>(vec2<T> const& v, U const& s) noexcept
{
    return vec2(v.x >> s, v.y >> s);
}

template<class T, class U> CONST_FUNC vec2<std::common_type_t<T, U>> operator + (T const& s, vec2<U> const& v) noexcept
{
    return vec2(s + v.x, s + v.y);
}
template<class T, class U> CONST_FUNC vec2<std::common_type_t<T, U>> operator - (T const& s, vec2<U> const& v) noexcept
{
    return vec2(s - v.x, s - v.y);
}
template<class T, class U> CONST_FUNC vec2<std::common_type_t<T, U>> operator * (T const& s, vec2<U> const& v) noexcept
{
    return vec2(s * v.x, s * v.y);
}
template<class T, class U> CONST_FUNC vec2<std::common_type_t<T, U>> operator / (T const& s, vec2<U> const& v) noexcept
{
    return vec2(s / v.x, s / v.y);
}
template<class T, class U> CONST_FUNC vec2<std::common_type_t<T, U>> operator % (T const& s, vec2<U> const& v) noexcept
{
    return vec2(s % v.x, s % v.y);
}
template<class T, class U> CONST_FUNC vec2<std::common_type_t<T, U>> operator | (T const& s, vec2<U> const& v) noexcept
{
    return vec2(s | v.x, s | v.y);
}
template<class T, class U> CONST_FUNC vec2<std::common_type_t<T, U>> operator & (T const& s, vec2<U> const& v) noexcept
{
    return vec2(s & v.x, s & v.y);
}
template<class T, class U> CONST_FUNC vec2<std::common_type_t<T, U>> operator ^ (T const& s, vec2<U> const& v) noexcept
{
    return vec2(s ^ v.x, s ^ v.y);
}
template<class T, class U> CONST_FUNC vec2<std::common_type_t<T, U>> operator <<(T const& s, vec2<U> const& v) noexcept
{
    return vec2(s << v.x, s << v.y);
}
template<class T, class U> CONST_FUNC vec2<std::common_type_t<T, U>> operator >>(T const& s, vec2<U> const& v) noexcept
{
    return vec2(s >> v.x, s >> v.y);
}

template<class T, class U> CONST_FUNC vec2<std::common_type_t<T, U>> operator + (vec2<T> const& v1, vec2<U> const& v2) noexcept
{
    return vec2(v1.x + v2.x, v1.y + v2.y);
}
template<class T, class U> CONST_FUNC vec2<std::common_type_t<T, U>> operator - (vec2<T> const& v1, vec2<U> const& v2) noexcept
{
    return vec2(v1.x - v2.x, v1.y - v2.y);
}
template<class T, class U> CONST_FUNC vec2<std::common_type_t<T, U>> operator * (vec2<T> const& v1, vec2<U> const& v2) noexcept
{
    return vec2(v1.x * v2.x, v1.y * v2.y);
}
template<class T, class U> CONST_FUNC vec2<std::common_type_t<T, U>> operator / (vec2<T> const& v1, vec2<U> const& v2) noexcept
{
    return vec2(v1.x / v2.x, v1.y / v2.y);
}
template<class T, class U> CONST_FUNC vec2<std::common_type_t<T, U>> operator % (vec2<T> const& v1, vec2<U> const& v2) noexcept
{
    return vec2(v1.x % v2.x, v1.y % v2.y);
}
template<class T, class U> CONST_FUNC vec2<std::common_type_t<T, U>> operator | (vec2<T> const& v1, vec2<U> const& v2) noexcept
{
    return vec2(v1.x | v2.x, v1.y | v2.y);
}
template<class T, class U> CONST_FUNC vec2<std::common_type_t<T, U>> operator & (vec2<T> const& v1, vec2<U> const& v2) noexcept
{
    return vec2(v1.x & v2.x, v1.y & v2.y);
}
template<class T, class U> CONST_FUNC vec2<std::common_type_t<T, U>> operator ^ (vec2<T> const& v1, vec2<U> const& v2) noexcept
{
    return vec2(v1.x ^ v2.x, v1.y ^ v2.y);
}
template<class T, class U> CONST_FUNC vec2<std::common_type_t<T, U>> operator <<(vec2<T> const& v1, vec2<U> const& v2) noexcept
{
    return vec2(v1.x << v2.x, v1.y << v2.y);
}
template<class T, class U> CONST_FUNC vec2<std::common_type_t<T, U>> operator >>(vec2<T> const& v1, vec2<U> const& v2) noexcept
{
    return vec2(v1.x >> v2.x, v1.y >> v2.y);
}

template<class T> CONST_FUNC vec2<T> min(vec2<T> const& v, T const& s) noexcept
{
    return vec2(std::min(v.x, s), std::min(v.y, s));
}
template<class T> CONST_FUNC vec2<T> max(vec2<T> const& v, T const& s) noexcept
{
    return vec2(std::max(v.x, s), std::max(v.y, s));
}
template<class T> CONST_FUNC vec2<T> min(T const& s, vec2<T> const& v) noexcept
{
    return vec2(std::min(s, v.x), std::min(s. v.y));
}
template<class T> CONST_FUNC vec2<T> max(T const& s, vec2<T> const& v) noexcept
{
    return vec2(std::max(s, v.x), std::max(s, v.y));
}
template<class T> CONST_FUNC vec2<T> min(vec2<T> const& v1, vec2<T> const& v2) noexcept
{
    return vec2(std::min(v1.x, v2.x), std::min(v1.y, v2.y));
}
template<class T> CONST_FUNC vec2<T> max(vec2<T> const& v1, vec2<T> const& v2) noexcept
{
    return vec2(std::max(v1.x, v2.x), std::max(v1.y, v2.y));
}
