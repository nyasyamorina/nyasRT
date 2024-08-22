#pragma once

#include <functional>
#include <math.h>
#include <tuple>
#include <type_traits>

#include "../utils.hpp"


template<class T> class vec2;

template<class T> class vec3 final
{
    static_assert(std::is_arithmetic_v<T> || std::is_same_v<T, bool>, "element type of vector must be number or boolean");

public:

    using eltype = T;

    T x, y, z;

    CONST_FUNC vec3() noexcept
    {}
    CONST_FUNC vec3(T const& s) noexcept
    : x(s), y(s), z(s) {}
    explicit CONST_FUNC vec3(T const& x_, T const& y_, T const& z_) noexcept
    : x(x_), y(y_), z(z_) {}
    explicit CONST_FUNC vec3(vec2<T> const& xy_, T const& z_) noexcept
    : x(xy_.x), y(xy_.y), z(z_) {}
    explicit CONST_FUNC vec3(T const& x_, vec2<T> const& yz_) noexcept
    : x(x_), y(yz_.x), z(yz_.y) {}
    CONST_FUNC vec3(vec3 const&) = default;

    template<class U> CONST_FUNC vec3(vec3<U> const& v) noexcept
    : x(v.x), y(v.y), z(v.z) {}
    template<class U> CONST_FUNC vec3 & operator = (vec3<U> const& v) noexcept
    {
        x = v.x; y = v.y; z = v.z;
        return *this;
    }


    /******** map to each elements ********/

    CONST_FUNC vec3 & apply(std::function<void(T &)> const& func) noexcept
    {
        func(x); func(y); func(z);
        return *this;
    }

    template<class U> CONST_FUNC vec3<U> map(std::function<U(T &)> const& func) noexcept
    {
        return vec3(func(x), func(y), func(z));
    }
    template<class U> CONST_FUNC vec3<U> map(std::function<U(T const&)> const& func) const noexcept
    {
        return vec3(func(x), func(y), func(z));
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
    CONST_FUNC vec2<T> zx() const noexcept
    {
        return vec2(z, x);
    }
    CONST_FUNC vec2<T> xy() const noexcept
    {
        return vec2(x, y);
    }
    CONST_FUNC vec2<T> yy() const noexcept
    {
        return vec2(y, y);
    }
    CONST_FUNC vec2<T> zy() const noexcept
    {
        return vec2(z, y);
    }
    CONST_FUNC vec2<T> xz() const noexcept
    {
        return vec2(x, z);
    }
    CONST_FUNC vec2<T> yz() const noexcept
    {
        return vec2(y, z);
    }
    CONST_FUNC vec2<T> zz() const noexcept
    {
        return vec2(z, z);
    }

    CONST_FUNC vec3<T> xxx() const noexcept
    {
        return vec3(x, x, x);
    }
    CONST_FUNC vec3<T> yxx() const noexcept
    {
        return vec3(y, x, x);
    }
    CONST_FUNC vec3<T> zxx() const noexcept
    {
        return vec3(z, x, x);
    }
    CONST_FUNC vec3<T> xyx() const noexcept
    {
        return vec3(x, y, x);
    }
    CONST_FUNC vec3<T> yyx() const noexcept
    {
        return vec3(y, y, x);
    }
    CONST_FUNC vec3<T> zyx() const noexcept
    {
        return vec3(z, y, x);
    }
    CONST_FUNC vec3<T> xzx() const noexcept
    {
        return vec3(x, z, x);
    }
    CONST_FUNC vec3<T> yzx() const noexcept
    {
        return vec3(y, z, x);
    }
    CONST_FUNC vec3<T> zzx() const noexcept
    {
        return vec3(z, z, x);
    }
    CONST_FUNC vec3<T> xxy() const noexcept
    {
        return vec3(x, x, y);
    }
    CONST_FUNC vec3<T> yxy() const noexcept
    {
        return vec3(y, x, y);
    }
    CONST_FUNC vec3<T> zxy() const noexcept
    {
        return vec3(z, x, y);
    }
    CONST_FUNC vec3<T> xyy() const noexcept
    {
        return vec3(x, y, y);
    }
    CONST_FUNC vec3<T> yyy() const noexcept
    {
        return vec3(y, y, y);
    }
    CONST_FUNC vec3<T> zyy() const noexcept
    {
        return vec3(z, y, y);
    }
    CONST_FUNC vec3<T> xzy() const noexcept
    {
        return vec3(x, z, y);
    }
    CONST_FUNC vec3<T> yzy() const noexcept
    {
        return vec3(y, z, y);
    }
    CONST_FUNC vec3<T> zzy() const noexcept
    {
        return vec3(z, z, y);
    }
    CONST_FUNC vec3<T> xxz() const noexcept
    {
        return vec3(x, x, z);
    }
    CONST_FUNC vec3<T> yxz() const noexcept
    {
        return vec3(y, x, z);
    }
    CONST_FUNC vec3<T> zxz() const noexcept
    {
        return vec3(z, x, z);
    }
    CONST_FUNC vec3<T> xyz() const noexcept
    {
        return vec3(x, y, z);
    }
    CONST_FUNC vec3<T> yyz() const noexcept
    {
        return vec3(y, y, z);
    }
    CONST_FUNC vec3<T> zyz() const noexcept
    {
        return vec3(z, y, z);
    }
    CONST_FUNC vec3<T> xzz() const noexcept
    {
        return vec3(x, z, z);
    }
    CONST_FUNC vec3<T> yzz() const noexcept
    {
        return vec3(y, z, z);
    }
    CONST_FUNC vec3<T> zzz() const noexcept
    {
        return vec3(z, z, z);
    }


    /******** inplace geometry operations ********/

    CONST_FUNC vec3 & normalize() noexcept
    {
        static_assert(std::is_floating_point_v<T>, "`normalize` must done in fp vectors.");
        T n = std::sqrt(x * x + y * y + z * z);
        (*this) /= n;
        return *this;
    }
    // normalize vector and return this length before normalized.
    CONST_FUNC T normlen() noexcept
    {
        static_assert(std::is_floating_point_v<T>, "`normlen` must done in fp vectors.");
        T n = std::sqrt(x * x + y * y + z * z);
        (*this) /= n;
        return n;
    }

    template<class U> CONST_FUNC vec3 & reflect(vec3<U> const& n) noexcept
    {
        auto a = x * n.x + y * n.y + z * n.z;
        constexpr decltype(a) two = static_cast<decltype(a)>(2);
        x -= two * a * n.x; y -= two * a * n.y; z -= two * a * n.z;
        return *this;
    }

    // *this = corss(v, *this)
    template<class U> CONST_FUNC vec3 & lcross(vec3<U> const& v) noexcept
    {
        T tmp_x = x, tmp_y = y;
        x = v.y *     z - v.z * tmp_y;
        y = v.z * tmp_x - v.x *     z;
        z = v.x * tmp_y - v.y * tmp_x;
        return *this;
    }
    // *this = corss(*this, v)
    template<class U> CONST_FUNC vec3 & rcross(vec3<U> const& v) noexcept
    {
        T tmp_x = x, tmp_y = y;
        x = tmp_y * v.z -     z * v.y;
        y =     z * v.x - tmp_x * v.z;
        z = tmp_x * v.y - tmp_y * v.x;
        return *this;
    }


    /******** inplace numerical operations ********/

    CONST_FUNC vec3 & neg() noexcept
    {
        if constexpr (std::is_same_v<T, bool>)
        {
            x = !x; y = !y; z = !z;
        }
        else
        {
            x = -x; y = -y; z = -z;
        }
        return *this;
    }

    template<class U> CONST_FUNC vec3 & operator += (U const& s) noexcept
    {
        x += s; y += s; z += s;
        return *this;
    }
    template<class U> CONST_FUNC vec3 & operator -= (U const& s) noexcept
    {
        x -= s; y -= s; z -= s;
        return *this;
    }
    template<class U> CONST_FUNC vec3 & operator *= (U const& s) noexcept
    {
        x *= s; y *= s; z *= s;
        return *this;
    }
    template<class U> CONST_FUNC vec3 & operator /= (U const& s) noexcept
    {
        x /= s; y /= s; z /= s;
        return *this;
    }
    template<class U> CONST_FUNC vec3 & operator %= (U const& s) noexcept
    {
        x %= s; y %= s; z %= s;
        return *this;
    }
    template<class U> CONST_FUNC vec3 & operator |= (U const& s) noexcept
    {
        x |= s; y |= s; z |= s;
        return *this;
    }
    template<class U> CONST_FUNC vec3 & operator &= (U const& s) noexcept
    {
        x &= s; y &= s; z &= s;
        return *this;
    }
    template<class U> CONST_FUNC vec3 & operator ^= (U const& s) noexcept
    {
        x ^= s; y ^= s; z ^= s;
        return *this;
    }
    template<class U> CONST_FUNC vec3 & operator <<=(U const& s) noexcept
    {
        x <<= s; y <<= s; z <<= s;
        return *this;
    }
    template<class U> CONST_FUNC vec3 & operator >>=(U const& s) noexcept
    {
        x >>= s; y >>= s; z >>= s;
        return *this;
    }

    template<class U> CONST_FUNC vec3 & operator += (vec3<U> const& v) noexcept
    {
        x += v.x; y += v.y; z += v.z;
        return *this;
    }
    template<class U> CONST_FUNC vec3 & operator -= (vec3<U> const& v) noexcept
    {
        x -= v.x; y -= v.y; z -= v.z;
        return *this;
    }
    template<class U> CONST_FUNC vec3 & operator *= (vec3<U> const& v) noexcept
    {
        x *= v.x; y *= v.y; z *= v.z;
        return *this;
    }
    template<class U> CONST_FUNC vec3 & operator /= (vec3<U> const& v) noexcept
    {
        x /= v.x; y /= v.y; z /= v.z;
        return *this;
    }
    template<class U> CONST_FUNC vec3 & operator %= (vec3<U> const& v) noexcept
    {
        x %= v.x; y %= v.y; z %= v.z;
        return *this;
    }
    template<class U> CONST_FUNC vec3 & operator |= (vec3<U> const& v) noexcept
    {
        x |= v.x; y |= v.y; z |= v.z;
        return *this;
    }
    template<class U> CONST_FUNC vec3 & operator &= (vec3<U> const& v) noexcept
    {
        x &= v.x; y &= v.y; z &= v.z;
        return *this;
    }
    template<class U> CONST_FUNC vec3 & operator ^= (vec3<U> const& v) noexcept
    {
        x ^= v.x; y ^= v.y; z ^= v.z;
        return *this;
    }
    template<class U> CONST_FUNC vec3 & operator <<=(vec3<U> const& v) noexcept
    {
        x <<= v.x; y <<= v.y; z <<= v.z;
        return *this;
    }
    template<class U> CONST_FUNC vec3 & operator >>=(vec3<U> const& v) noexcept
    {
        x >>= v.x; y >>= v.y; z >>= v.z;
        return *this;
    }

    template<class U> CONST_FUNC vec3 & add(U const& other) noexcept
    {
        (*this) += other;
        return *this;
    }
    template<class U> CONST_FUNC vec3 & sub(U const& other) noexcept
    {
        (*this) -= other;
        return *this;
    }
    template<class U> CONST_FUNC vec3 & mul(U const& other) noexcept
    {
        (*this) *= other;
        return *this;
    }
    template<class U> CONST_FUNC vec3 & div(U const& other) noexcept
    {
        (*this) /= other;
        return *this;
    }
    template<class U> CONST_FUNC vec3 & mod(U const& other) noexcept
    {
        (*this) &= other;
        return *this;
    }

    /******** compare each elements operations ********/

    template <class U> CONST_FUNC vec3<bool> isless(U const& s) const noexcept
    {
        return vec3(x < s, y < s, z < s);
    }
    template <class U> CONST_FUNC vec3<bool> isleq(U const& s) const noexcept
    {
        return vec3(x <= s, y <= s, z <= s);
    }
    template <class U> CONST_FUNC vec3<bool> iseq(U const& s) const noexcept
    {
        return vec3(x == s, y == s, z == s);
    }
    template <class U> CONST_FUNC vec3<bool> isneq(U const& s) const noexcept
    {
        return vec3(x != s, y != s, z != s);
    }
    template <class U> CONST_FUNC vec3<bool> isgeq(U const& s) const noexcept
    {
        return vec3(x >= s, y >= s, z >= s);
    }
    template <class U> CONST_FUNC vec3<bool> isgreater(U const& s) const noexcept
    {
        return vec3(x > s, y > s, z > s);
    }

    template <class U> CONST_FUNC vec3<bool> isless(vec3<U> const& v) const noexcept
    {
        return vec3(x < v.x, y < v.y, z < v.z);
    }
    template <class U> CONST_FUNC vec3<bool> isleq(vec3<U> const& v) const noexcept
    {
        return vec3(x <= v.x, y <= v.y, z <= v.z);
    }
    template <class U> CONST_FUNC vec3<bool> iseq(vec3<U> const& v) const noexcept
    {
        return vec3(x == v.x, y == v.y, z == v.z);
    }
    template <class U> CONST_FUNC vec3<bool> isneq(vec3<U> const& v) const noexcept
    {
        return vec3(x != v.x, y != v.y, z != v.z);
    }
    template <class U> CONST_FUNC vec3<bool> isgeq(vec3<U> const& v) const noexcept
    {
        return vec3(x >= v.x, y >= v.y, z >= v.z);
    }
    template <class U> CONST_FUNC vec3<bool> isgreater(vec3<U> const& v) const noexcept
    {
        return vec3(x > v.x, y > v.y, z > v.z);
    }
};

// the vector type used in geometry calculations
using vec3g = vec3<fg>;


/********** default values **********/

template<class T> class defaults<vec3<T>>
{
public:

    static constexpr vec3<T> O = vec3<T>(static_cast<T>(0), static_cast<T>(0), static_cast<T>(0));
    static constexpr vec3<T> X = vec3<T>(static_cast<T>(1), static_cast<T>(0), static_cast<T>(0));
    static constexpr vec3<T> Y = vec3<T>(static_cast<T>(0), static_cast<T>(1), static_cast<T>(0));
    static constexpr vec3<T> Z = vec3<T>(static_cast<T>(0), static_cast<T>(0), static_cast<T>(1));
    static constexpr vec3<T> min = vec3<T>(std::numeric_limits<T>::min());
    static constexpr vec3<T> max = vec3<T>(std::numeric_limits<T>::max());
};


/********** geometry operations **********/

template<class T, class U> CONST_FUNC std::common_type_t<T, U> dot(vec3<T> const& v1, vec3<U> const& v2) noexcept
{
    return v1.x * v2.x + v1.y * v2.y + v1.z * v2.z;
}

template<class T> CONST_FUNC T length2(vec3<T> const& v) noexcept
{
    return dot(v, v);
}
template<class T> CONST_FUNC T length(vec3<T> const& v) noexcept
{
    static_assert(std::is_floating_point_v<T>, "`length` must done in fp vectors.");
    return std::sqrt(length2(v));
}

template<class T> CONST_FUNC vec3<T> normalize(vec3<T> const& v) noexcept
{
    static_assert(std::is_floating_point_v<T>, "`normalize` must done in fp vectors.");
    T n = length(v);
    return vec3(v.x / n, v.y / n, v.z / n);
}
// normalized vector and the length of input vector
template<class T> CONST_FUNC std::tuple<vec3<T>, T> normlen(vec3<T> const& v) noexcept
{
    static_assert(std::is_floating_point_v<T>, "`normlen` must done in fp vectors.");
    T n = length(v);
    return {vec3(v.x / n, v.y / n, v.z / n), n};
}

template<class T, class U> vec3<std::common_type_t<T, U>> reflect(vec3<T> const& v, vec3<U> const& n) noexcept
{
    return v - static_cast<std::common_type_t<T, U>>(2) * dot(v, n) * n;
}

template<class T, class U> vec3<std::common_type_t<T, U>> cross(vec3<T> const& v1, vec3<U> const& v2) noexcept
{
    return vec3
    (
        v1.y * v2.z - v1.z * v2.y,
        v1.z * v2.x - v1.x * v2.z,
        v1.x * v2.y - v1.y * v2.x
    );
}


/******** numerical operations ********/

template<class T> CONST_FUNC T sum(vec3<T> const& v) noexcept
{
    return v.x + v.y + v.z;
}

template<class T> CONST_FUNC vec3<T> operator + (vec3<T> const& v) noexcept
{
    return vec3(+v.x, +v.y, +v.z);
}
template<class T> CONST_FUNC vec3<T> operator - (vec3<T> const& v) noexcept
{
    return vec3(-v.x, -v.y, -v.z);
}
template<class T> CONST_FUNC vec3<T> operator ~ (vec3<T> const& v) noexcept
{
    return vec3(~v.x, ~v.y, ~v.z);
}
CONST_FUNC vec3<bool> operator ! (vec3<bool> const& v) noexcept
{
    return vec3(!v.x, !v.y, !v.z);
}

CONST_FUNC bool all(vec3<bool> const& v) noexcept
{
    return v.x && v.y && v.z;
}
CONST_FUNC bool any(vec3<bool> const& v) noexcept
{
    return v.x || v.y || v.z;
}
CONST_FUNC bool not_all_equal(vec3<bool> const& v) noexcept
{
    return ((!v.x) != (!v.y)) || ((!v.y) != (!v.z));
}

template<class T, class U> CONST_FUNC vec3<std::common_type_t<T, U>> operator + (vec3<T> const& v, U const& s) noexcept
{
    return vec3(v.x + s, v.y + s, v.z + s);
}
template<class T, class U> CONST_FUNC vec3<std::common_type_t<T, U>> operator - (vec3<T> const& v, U const& s) noexcept
{
    return vec3(v.x - s, v.y - s, v.z - s);
}
template<class T, class U> CONST_FUNC vec3<std::common_type_t<T, U>> operator * (vec3<T> const& v, U const& s) noexcept
{
    return vec3(v.x * s, v.y * s, v.z * s);
}
template<class T, class U> CONST_FUNC vec3<std::common_type_t<T, U>> operator / (vec3<T> const& v, U const& s) noexcept
{
    return vec3(v.x / s, v.y / s, v.z / s);
}
template<class T, class U> CONST_FUNC vec3<std::common_type_t<T, U>> operator % (vec3<T> const& v, U const& s) noexcept
{
    return vec3(v.x % s, v.y % s, v.z % s);
}
template<class T, class U> CONST_FUNC vec3<std::common_type_t<T, U>> operator | (vec3<T> const& v, U const& s) noexcept
{
    return vec3(v.x | s, v.y | s, v.z | s);
}
template<class T, class U> CONST_FUNC vec3<std::common_type_t<T, U>> operator & (vec3<T> const& v, U const& s) noexcept
{
    return vec3(v.x & s, v.y & s, v.z & s);
}
template<class T, class U> CONST_FUNC vec3<std::common_type_t<T, U>> operator ^ (vec3<T> const& v, U const& s) noexcept
{
    return vec3(v.x ^ s, v.y ^ s, v.z ^ s);
}
template<class T, class U> CONST_FUNC vec3<std::common_type_t<T, U>> operator <<(vec3<T> const& v, U const& s) noexcept
{
    return vec3(v.x << s, v.y << s, v.z << s);
}
template<class T, class U> CONST_FUNC vec3<std::common_type_t<T, U>> operator >>(vec3<T> const& v, U const& s) noexcept
{
    return vec3(v.x >> s, v.y >> s, v.z >> s);
}

template<class T, class U> CONST_FUNC vec3<std::common_type_t<T, U>> operator + (T const& s, vec3<U> const& v) noexcept
{
    return vec3(s + v.x, s + v.y, s + v.z);
}
template<class T, class U> CONST_FUNC vec3<std::common_type_t<T, U>> operator - (T const& s, vec3<U> const& v) noexcept
{
    return vec3(s - v.x, s - v.y, s - v.z);
}
template<class T, class U> CONST_FUNC vec3<std::common_type_t<T, U>> operator * (T const& s, vec3<U> const& v) noexcept
{
    return vec3(s * v.x, s * v.y, s * v.z);
}
template<class T, class U> CONST_FUNC vec3<std::common_type_t<T, U>> operator / (T const& s, vec3<U> const& v) noexcept
{
    return vec3(s / v.x, s / v.y, s / v.z);
}
template<class T, class U> CONST_FUNC vec3<std::common_type_t<T, U>> operator % (T const& s, vec3<U> const& v) noexcept
{
    return vec3(s % v.x, s % v.y, s % v.z);
}
template<class T, class U> CONST_FUNC vec3<std::common_type_t<T, U>> operator | (T const& s, vec3<U> const& v) noexcept
{
    return vec3(s | v.x, s | v.y, s | v.z);
}
template<class T, class U> CONST_FUNC vec3<std::common_type_t<T, U>> operator & (T const& s, vec3<U> const& v) noexcept
{
    return vec3(s & v.x, s & v.y, s & v.z);
}
template<class T, class U> CONST_FUNC vec3<std::common_type_t<T, U>> operator ^ (T const& s, vec3<U> const& v) noexcept
{
    return vec3(s ^ v.x, s ^ v.y, s ^ v.z);
}
template<class T, class U> CONST_FUNC vec3<std::common_type_t<T, U>> operator <<(T const& s, vec3<U> const& v) noexcept
{
    return vec3(s << v.x, s << v.y, s << v.z);
}
template<class T, class U> CONST_FUNC vec3<std::common_type_t<T, U>> operator >>(T const& s, vec3<U> const& v) noexcept
{
    return vec3(s >> v.x, s >> v.y, s >> v.z);
}

template<class T, class U> CONST_FUNC vec3<std::common_type_t<T, U>> operator + (vec3<T> const& v1, vec3<U> const& v2) noexcept
{
    return vec3(v1.x + v2.x, v1.y + v2.y, v1.z + v2.z);
}
template<class T, class U> CONST_FUNC vec3<std::common_type_t<T, U>> operator - (vec3<T> const& v1, vec3<U> const& v2) noexcept
{
    return vec3(v1.x - v2.x, v1.y - v2.y, v1.z - v2.z);
}
template<class T, class U> CONST_FUNC vec3<std::common_type_t<T, U>> operator * (vec3<T> const& v1, vec3<U> const& v2) noexcept
{
    return vec3(v1.x * v2.x, v1.y * v2.y, v1.z * v2.z);
}
template<class T, class U> CONST_FUNC vec3<std::common_type_t<T, U>> operator / (vec3<T> const& v1, vec3<U> const& v2) noexcept
{
    return vec3(v1.x / v2.x, v1.y / v2.y, v1.z / v2.z);
}
template<class T, class U> CONST_FUNC vec3<std::common_type_t<T, U>> operator % (vec3<T> const& v1, vec3<U> const& v2) noexcept
{
    return vec3(v1.x % v2.x, v1.y % v2.y, v1.z % v2.z);
}
template<class T, class U> CONST_FUNC vec3<std::common_type_t<T, U>> operator | (vec3<T> const& v1, vec3<U> const& v2) noexcept
{
    return vec3(v1.x | v2.x, v1.y | v2.y, v1.z | v2.z);
}
template<class T, class U> CONST_FUNC vec3<std::common_type_t<T, U>> operator & (vec3<T> const& v1, vec3<U> const& v2) noexcept
{
    return vec3(v1.x & v2.x, v1.y & v2.y, v1.z & v2.z);
}
template<class T, class U> CONST_FUNC vec3<std::common_type_t<T, U>> operator ^ (vec3<T> const& v1, vec3<U> const& v2) noexcept
{
    return vec3(v1.x ^ v2.x, v1.y ^ v2.y, v1.z ^ v2.z);
}
template<class T, class U> CONST_FUNC vec3<std::common_type_t<T, U>> operator <<(vec3<T> const& v1, vec3<U> const& v2) noexcept
{
    return vec3(v1.x << v2.x, v1.y << v2.y, v1.z << v2.z);
}
template<class T, class U> CONST_FUNC vec3<std::common_type_t<T, U>> operator >>(vec3<T> const& v1, vec3<U> const& v2) noexcept
{
    return vec3(v1.x >> v2.x, v1.y >> v2.y, v1.z >> v2.z);
}
