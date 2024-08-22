#pragma once

#include <functional>
#include <math.h>
#include <tuple>
#include <type_traits>

#include "../utils.hpp"


template<class T> class vec2;
template<class T> class vec3;

template<class T> class vec4 final
{
    static_assert(std::is_arithmetic_v<T> || std::is_same_v<T, bool>, "element type of vector must be number or boolean");

public:

    using eltype = T;

    T x, y, z, w;

    CONST_FUNC vec4() noexcept
    {}
    CONST_FUNC vec4(T const& s) noexcept
    : x(s), y(s), z(s), w(s) {}
    explicit CONST_FUNC vec4(T const& x_, T const& y_, T const& z_, T const& w_) noexcept
    : x(x_), y(y_), z(z_), w(w_) {}
    explicit CONST_FUNC vec4(vec2<T> const& xy_, T const& z_, T const& w_) noexcept
    : x(xy_.x), y(xy_.y), z(z_), w(w_) {}
    explicit CONST_FUNC vec4(T const& x_, vec2<T> const& yz_, T const& w_) noexcept
    : x(x_), y(yz_.x), z(yz_.y), w(w_) {}
    explicit CONST_FUNC vec4(T const& x_, T const& y_, vec2<T> const& zw_) noexcept
    : x(x_), y(y_), z(zw_.x), w(zw_.y) {}
    explicit CONST_FUNC vec4(vec3<T> const& xyz_, T const& w_) noexcept
    : x(xyz_.x), y(xyz_.y), z(xyz_.z), w(w_) {}
    explicit CONST_FUNC vec4(T const& x_, vec3<T> const& yzw_) noexcept
    : x(x_), y(yzw_.x), z(yzw_.y), w(yzw_.z) {}
    CONST_FUNC vec4(vec4 const&) = default;

    template<class U> CONST_FUNC vec4(vec4<U> const& v) noexcept
    : x(v.x), y(v.y), z(v.z), w(v.w) {}
    template<class U> CONST_FUNC vec4 & operator = (vec4<U> const& v) noexcept
    {
        x = v.x; y = v.y; z = v.z; w = v.w;
        return *this;
    }


    /******** map to each elements ********/

    CONST_FUNC vec4 & apply(std::function<void(T &)> const& func) noexcept
    {
        func(x); func(y); func(z); func(w);
        return *this;
    }

    template<class U> CONST_FUNC vec4<U> map(std::function<U(T &)> const& func) noexcept
    {
        return vec4(func(x), func(y), func(z), func(w));
    }
    template<class U> CONST_FUNC vec4<U> map(std::function<U(T const&)> const& func) const noexcept
    {
        return vec4(func(x), func(y), func(z), func(w));
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
    CONST_FUNC vec2<T> wx() const noexcept
    {
        return vec2(w, x);
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
    CONST_FUNC vec2<T> wy() const noexcept
    {
        return vec2(w, y);
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
    CONST_FUNC vec2<T> wz() const noexcept
    {
        return vec2(w, z);
    }
    CONST_FUNC vec2<T> xw() const noexcept
    {
        return vec2(x, w);
    }
    CONST_FUNC vec2<T> yw() const noexcept
    {
        return vec2(y, w);
    }
    CONST_FUNC vec2<T> zw() const noexcept
    {
        return vec2(z, w);
    }
    CONST_FUNC vec2<T> ww() const noexcept
    {
        return vec2(w, w);
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
    CONST_FUNC vec3<T> wxx() const noexcept
    {
        return vec3(w, x, x);
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
    CONST_FUNC vec3<T> wyx() const noexcept
    {
        return vec3(w, y, x);
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
    CONST_FUNC vec3<T> wzx() const noexcept
    {
        return vec3(w, z, x);
    }
    CONST_FUNC vec3<T> xwx() const noexcept
    {
        return vec3(x, w, x);
    }
    CONST_FUNC vec3<T> ywx() const noexcept
    {
        return vec3(y, w, x);
    }
    CONST_FUNC vec3<T> zwx() const noexcept
    {
        return vec3(z, w, x);
    }
    CONST_FUNC vec3<T> wwx() const noexcept
    {
        return vec3(w, w, x);
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
    CONST_FUNC vec3<T> wxy() const noexcept
    {
        return vec3(w, x, y);
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
    CONST_FUNC vec3<T> wyy() const noexcept
    {
        return vec3(w, y, y);
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
    CONST_FUNC vec3<T> wzy() const noexcept
    {
        return vec3(w, z, y);
    }
    CONST_FUNC vec3<T> xwy() const noexcept
    {
        return vec3(x, w, y);
    }
    CONST_FUNC vec3<T> ywy() const noexcept
    {
        return vec3(y, w, y);
    }
    CONST_FUNC vec3<T> zwy() const noexcept
    {
        return vec3(z, w, y);
    }
    CONST_FUNC vec3<T> wwy() const noexcept
    {
        return vec3(w, w, y);
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
    CONST_FUNC vec3<T> wxz() const noexcept
    {
        return vec3(w, x, z);
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
    CONST_FUNC vec3<T> wyz() const noexcept
    {
        return vec3(w, y, z);
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
    CONST_FUNC vec3<T> wzz() const noexcept
    {
        return vec3(w, z, z);
    }
    CONST_FUNC vec3<T> xwz() const noexcept
    {
        return vec3(x, w, z);
    }
    CONST_FUNC vec3<T> ywz() const noexcept
    {
        return vec3(y, w, z);
    }
    CONST_FUNC vec3<T> zwz() const noexcept
    {
        return vec3(z, w, z);
    }
    CONST_FUNC vec3<T> wwz() const noexcept
    {
        return vec3(w, w, z);
    }
    CONST_FUNC vec3<T> xxw() const noexcept
    {
        return vec3(x, x, w);
    }
    CONST_FUNC vec3<T> yxw() const noexcept
    {
        return vec3(y, x, w);
    }
    CONST_FUNC vec3<T> zxw() const noexcept
    {
        return vec3(z, x, w);
    }
    CONST_FUNC vec3<T> wxw() const noexcept
    {
        return vec3(w, x, w);
    }
    CONST_FUNC vec3<T> xyw() const noexcept
    {
        return vec3(x, y, w);
    }
    CONST_FUNC vec3<T> yyw() const noexcept
    {
        return vec3(y, y, w);
    }
    CONST_FUNC vec3<T> zyw() const noexcept
    {
        return vec3(z, y, w);
    }
    CONST_FUNC vec3<T> wyw() const noexcept
    {
        return vec3(w, y, w);
    }
    CONST_FUNC vec3<T> xzw() const noexcept
    {
        return vec3(x, z, w);
    }
    CONST_FUNC vec3<T> yzw() const noexcept
    {
        return vec3(y, z, w);
    }
    CONST_FUNC vec3<T> zzw() const noexcept
    {
        return vec3(z, z, w);
    }
    CONST_FUNC vec3<T> wzw() const noexcept
    {
        return vec3(w, z, w);
    }
    CONST_FUNC vec3<T> xww() const noexcept
    {
        return vec3(x, w, w);
    }
    CONST_FUNC vec3<T> yww() const noexcept
    {
        return vec3(y, w, w);
    }
    CONST_FUNC vec3<T> zww() const noexcept
    {
        return vec3(z, w, w);
    }
    CONST_FUNC vec3<T> www() const noexcept
    {
        return vec3(w, w, w);
    }

    CONST_FUNC vec4<T> xxxx() const noexcept
    {
        return vec4(x, x, x, x);
    }
    CONST_FUNC vec4<T> yxxx() const noexcept
    {
        return vec4(y, x, x, x);
    }
    CONST_FUNC vec4<T> zxxx() const noexcept
    {
        return vec4(z, x, x, x);
    }
    CONST_FUNC vec4<T> wxxx() const noexcept
    {
        return vec4(w, x, x, x);
    }
    CONST_FUNC vec4<T> xyxx() const noexcept
    {
        return vec4(x, y, x, x);
    }
    CONST_FUNC vec4<T> yyxx() const noexcept
    {
        return vec4(y, y, x, x);
    }
    CONST_FUNC vec4<T> zyxx() const noexcept
    {
        return vec4(z, y, x, x);
    }
    CONST_FUNC vec4<T> wyxx() const noexcept
    {
        return vec4(w, y, x, x);
    }
    CONST_FUNC vec4<T> xzxx() const noexcept
    {
        return vec4(x, z, x, x);
    }
    CONST_FUNC vec4<T> yzxx() const noexcept
    {
        return vec4(y, z, x, x);
    }
    CONST_FUNC vec4<T> zzxx() const noexcept
    {
        return vec4(z, z, x, x);
    }
    CONST_FUNC vec4<T> wzxx() const noexcept
    {
        return vec4(w, z, x, x);
    }
    CONST_FUNC vec4<T> xwxx() const noexcept
    {
        return vec4(x, w, x, x);
    }
    CONST_FUNC vec4<T> ywxx() const noexcept
    {
        return vec4(y, w, x, x);
    }
    CONST_FUNC vec4<T> zwxx() const noexcept
    {
        return vec4(z, w, x, x);
    }
    CONST_FUNC vec4<T> wwxx() const noexcept
    {
        return vec4(w, w, x, x);
    }
    CONST_FUNC vec4<T> xxyx() const noexcept
    {
        return vec4(x, x, y, x);
    }
    CONST_FUNC vec4<T> yxyx() const noexcept
    {
        return vec4(y, x, y, x);
    }
    CONST_FUNC vec4<T> zxyx() const noexcept
    {
        return vec4(z, x, y, x);
    }
    CONST_FUNC vec4<T> wxyx() const noexcept
    {
        return vec4(w, x, y, x);
    }
    CONST_FUNC vec4<T> xyyx() const noexcept
    {
        return vec4(x, y, y, x);
    }
    CONST_FUNC vec4<T> yyyx() const noexcept
    {
        return vec4(y, y, y, x);
    }
    CONST_FUNC vec4<T> zyyx() const noexcept
    {
        return vec4(z, y, y, x);
    }
    CONST_FUNC vec4<T> wyyx() const noexcept
    {
        return vec4(w, y, y, x);
    }
    CONST_FUNC vec4<T> xzyx() const noexcept
    {
        return vec4(x, z, y, x);
    }
    CONST_FUNC vec4<T> yzyx() const noexcept
    {
        return vec4(y, z, y, x);
    }
    CONST_FUNC vec4<T> zzyx() const noexcept
    {
        return vec4(z, z, y, x);
    }
    CONST_FUNC vec4<T> wzyx() const noexcept
    {
        return vec4(w, z, y, x);
    }
    CONST_FUNC vec4<T> xwyx() const noexcept
    {
        return vec4(x, w, y, x);
    }
    CONST_FUNC vec4<T> ywyx() const noexcept
    {
        return vec4(y, w, y, x);
    }
    CONST_FUNC vec4<T> zwyx() const noexcept
    {
        return vec4(z, w, y, x);
    }
    CONST_FUNC vec4<T> wwyx() const noexcept
    {
        return vec4(w, w, y, x);
    }
    CONST_FUNC vec4<T> xxzx() const noexcept
    {
        return vec4(x, x, z, x);
    }
    CONST_FUNC vec4<T> yxzx() const noexcept
    {
        return vec4(y, x, z, x);
    }
    CONST_FUNC vec4<T> zxzx() const noexcept
    {
        return vec4(z, x, z, x);
    }
    CONST_FUNC vec4<T> wxzx() const noexcept
    {
        return vec4(w, x, z, x);
    }
    CONST_FUNC vec4<T> xyzx() const noexcept
    {
        return vec4(x, y, z, x);
    }
    CONST_FUNC vec4<T> yyzx() const noexcept
    {
        return vec4(y, y, z, x);
    }
    CONST_FUNC vec4<T> zyzx() const noexcept
    {
        return vec4(z, y, z, x);
    }
    CONST_FUNC vec4<T> wyzx() const noexcept
    {
        return vec4(w, y, z, x);
    }
    CONST_FUNC vec4<T> xzzx() const noexcept
    {
        return vec4(x, z, z, x);
    }
    CONST_FUNC vec4<T> yzzx() const noexcept
    {
        return vec4(y, z, z, x);
    }
    CONST_FUNC vec4<T> zzzx() const noexcept
    {
        return vec4(z, z, z, x);
    }
    CONST_FUNC vec4<T> wzzx() const noexcept
    {
        return vec4(w, z, z, x);
    }
    CONST_FUNC vec4<T> xwzx() const noexcept
    {
        return vec4(x, w, z, x);
    }
    CONST_FUNC vec4<T> ywzx() const noexcept
    {
        return vec4(y, w, z, x);
    }
    CONST_FUNC vec4<T> zwzx() const noexcept
    {
        return vec4(z, w, z, x);
    }
    CONST_FUNC vec4<T> wwzx() const noexcept
    {
        return vec4(w, w, z, x);
    }
    CONST_FUNC vec4<T> xxwx() const noexcept
    {
        return vec4(x, x, w, x);
    }
    CONST_FUNC vec4<T> yxwx() const noexcept
    {
        return vec4(y, x, w, x);
    }
    CONST_FUNC vec4<T> zxwx() const noexcept
    {
        return vec4(z, x, w, x);
    }
    CONST_FUNC vec4<T> wxwx() const noexcept
    {
        return vec4(w, x, w, x);
    }
    CONST_FUNC vec4<T> xywx() const noexcept
    {
        return vec4(x, y, w, x);
    }
    CONST_FUNC vec4<T> yywx() const noexcept
    {
        return vec4(y, y, w, x);
    }
    CONST_FUNC vec4<T> zywx() const noexcept
    {
        return vec4(z, y, w, x);
    }
    CONST_FUNC vec4<T> wywx() const noexcept
    {
        return vec4(w, y, w, x);
    }
    CONST_FUNC vec4<T> xzwx() const noexcept
    {
        return vec4(x, z, w, x);
    }
    CONST_FUNC vec4<T> yzwx() const noexcept
    {
        return vec4(y, z, w, x);
    }
    CONST_FUNC vec4<T> zzwx() const noexcept
    {
        return vec4(z, z, w, x);
    }
    CONST_FUNC vec4<T> wzwx() const noexcept
    {
        return vec4(w, z, w, x);
    }
    CONST_FUNC vec4<T> xwwx() const noexcept
    {
        return vec4(x, w, w, x);
    }
    CONST_FUNC vec4<T> ywwx() const noexcept
    {
        return vec4(y, w, w, x);
    }
    CONST_FUNC vec4<T> zwwx() const noexcept
    {
        return vec4(z, w, w, x);
    }
    CONST_FUNC vec4<T> wwwx() const noexcept
    {
        return vec4(w, w, w, x);
    }
    CONST_FUNC vec4<T> xxxy() const noexcept
    {
        return vec4(x, x, x, y);
    }
    CONST_FUNC vec4<T> yxxy() const noexcept
    {
        return vec4(y, x, x, y);
    }
    CONST_FUNC vec4<T> zxxy() const noexcept
    {
        return vec4(z, x, x, y);
    }
    CONST_FUNC vec4<T> wxxy() const noexcept
    {
        return vec4(w, x, x, y);
    }
    CONST_FUNC vec4<T> xyxy() const noexcept
    {
        return vec4(x, y, x, y);
    }
    CONST_FUNC vec4<T> yyxy() const noexcept
    {
        return vec4(y, y, x, y);
    }
    CONST_FUNC vec4<T> zyxy() const noexcept
    {
        return vec4(z, y, x, y);
    }
    CONST_FUNC vec4<T> wyxy() const noexcept
    {
        return vec4(w, y, x, y);
    }
    CONST_FUNC vec4<T> xzxy() const noexcept
    {
        return vec4(x, z, x, y);
    }
    CONST_FUNC vec4<T> yzxy() const noexcept
    {
        return vec4(y, z, x, y);
    }
    CONST_FUNC vec4<T> zzxy() const noexcept
    {
        return vec4(z, z, x, y);
    }
    CONST_FUNC vec4<T> wzxy() const noexcept
    {
        return vec4(w, z, x, y);
    }
    CONST_FUNC vec4<T> xwxy() const noexcept
    {
        return vec4(x, w, x, y);
    }
    CONST_FUNC vec4<T> ywxy() const noexcept
    {
        return vec4(y, w, x, y);
    }
    CONST_FUNC vec4<T> zwxy() const noexcept
    {
        return vec4(z, w, x, y);
    }
    CONST_FUNC vec4<T> wwxy() const noexcept
    {
        return vec4(w, w, x, y);
    }
    CONST_FUNC vec4<T> xxyy() const noexcept
    {
        return vec4(x, x, y, y);
    }
    CONST_FUNC vec4<T> yxyy() const noexcept
    {
        return vec4(y, x, y, y);
    }
    CONST_FUNC vec4<T> zxyy() const noexcept
    {
        return vec4(z, x, y, y);
    }
    CONST_FUNC vec4<T> wxyy() const noexcept
    {
        return vec4(w, x, y, y);
    }
    CONST_FUNC vec4<T> xyyy() const noexcept
    {
        return vec4(x, y, y, y);
    }
    CONST_FUNC vec4<T> yyyy() const noexcept
    {
        return vec4(y, y, y, y);
    }
    CONST_FUNC vec4<T> zyyy() const noexcept
    {
        return vec4(z, y, y, y);
    }
    CONST_FUNC vec4<T> wyyy() const noexcept
    {
        return vec4(w, y, y, y);
    }
    CONST_FUNC vec4<T> xzyy() const noexcept
    {
        return vec4(x, z, y, y);
    }
    CONST_FUNC vec4<T> yzyy() const noexcept
    {
        return vec4(y, z, y, y);
    }
    CONST_FUNC vec4<T> zzyy() const noexcept
    {
        return vec4(z, z, y, y);
    }
    CONST_FUNC vec4<T> wzyy() const noexcept
    {
        return vec4(w, z, y, y);
    }
    CONST_FUNC vec4<T> xwyy() const noexcept
    {
        return vec4(x, w, y, y);
    }
    CONST_FUNC vec4<T> ywyy() const noexcept
    {
        return vec4(y, w, y, y);
    }
    CONST_FUNC vec4<T> zwyy() const noexcept
    {
        return vec4(z, w, y, y);
    }
    CONST_FUNC vec4<T> wwyy() const noexcept
    {
        return vec4(w, w, y, y);
    }
    CONST_FUNC vec4<T> xxzy() const noexcept
    {
        return vec4(x, x, z, y);
    }
    CONST_FUNC vec4<T> yxzy() const noexcept
    {
        return vec4(y, x, z, y);
    }
    CONST_FUNC vec4<T> zxzy() const noexcept
    {
        return vec4(z, x, z, y);
    }
    CONST_FUNC vec4<T> wxzy() const noexcept
    {
        return vec4(w, x, z, y);
    }
    CONST_FUNC vec4<T> xyzy() const noexcept
    {
        return vec4(x, y, z, y);
    }
    CONST_FUNC vec4<T> yyzy() const noexcept
    {
        return vec4(y, y, z, y);
    }
    CONST_FUNC vec4<T> zyzy() const noexcept
    {
        return vec4(z, y, z, y);
    }
    CONST_FUNC vec4<T> wyzy() const noexcept
    {
        return vec4(w, y, z, y);
    }
    CONST_FUNC vec4<T> xzzy() const noexcept
    {
        return vec4(x, z, z, y);
    }
    CONST_FUNC vec4<T> yzzy() const noexcept
    {
        return vec4(y, z, z, y);
    }
    CONST_FUNC vec4<T> zzzy() const noexcept
    {
        return vec4(z, z, z, y);
    }
    CONST_FUNC vec4<T> wzzy() const noexcept
    {
        return vec4(w, z, z, y);
    }
    CONST_FUNC vec4<T> xwzy() const noexcept
    {
        return vec4(x, w, z, y);
    }
    CONST_FUNC vec4<T> ywzy() const noexcept
    {
        return vec4(y, w, z, y);
    }
    CONST_FUNC vec4<T> zwzy() const noexcept
    {
        return vec4(z, w, z, y);
    }
    CONST_FUNC vec4<T> wwzy() const noexcept
    {
        return vec4(w, w, z, y);
    }
    CONST_FUNC vec4<T> xxwy() const noexcept
    {
        return vec4(x, x, w, y);
    }
    CONST_FUNC vec4<T> yxwy() const noexcept
    {
        return vec4(y, x, w, y);
    }
    CONST_FUNC vec4<T> zxwy() const noexcept
    {
        return vec4(z, x, w, y);
    }
    CONST_FUNC vec4<T> wxwy() const noexcept
    {
        return vec4(w, x, w, y);
    }
    CONST_FUNC vec4<T> xywy() const noexcept
    {
        return vec4(x, y, w, y);
    }
    CONST_FUNC vec4<T> yywy() const noexcept
    {
        return vec4(y, y, w, y);
    }
    CONST_FUNC vec4<T> zywy() const noexcept
    {
        return vec4(z, y, w, y);
    }
    CONST_FUNC vec4<T> wywy() const noexcept
    {
        return vec4(w, y, w, y);
    }
    CONST_FUNC vec4<T> xzwy() const noexcept
    {
        return vec4(x, z, w, y);
    }
    CONST_FUNC vec4<T> yzwy() const noexcept
    {
        return vec4(y, z, w, y);
    }
    CONST_FUNC vec4<T> zzwy() const noexcept
    {
        return vec4(z, z, w, y);
    }
    CONST_FUNC vec4<T> wzwy() const noexcept
    {
        return vec4(w, z, w, y);
    }
    CONST_FUNC vec4<T> xwwy() const noexcept
    {
        return vec4(x, w, w, y);
    }
    CONST_FUNC vec4<T> ywwy() const noexcept
    {
        return vec4(y, w, w, y);
    }
    CONST_FUNC vec4<T> zwwy() const noexcept
    {
        return vec4(z, w, w, y);
    }
    CONST_FUNC vec4<T> wwwy() const noexcept
    {
        return vec4(w, w, w, y);
    }
    CONST_FUNC vec4<T> xxxz() const noexcept
    {
        return vec4(x, x, x, z);
    }
    CONST_FUNC vec4<T> yxxz() const noexcept
    {
        return vec4(y, x, x, z);
    }
    CONST_FUNC vec4<T> zxxz() const noexcept
    {
        return vec4(z, x, x, z);
    }
    CONST_FUNC vec4<T> wxxz() const noexcept
    {
        return vec4(w, x, x, z);
    }
    CONST_FUNC vec4<T> xyxz() const noexcept
    {
        return vec4(x, y, x, z);
    }
    CONST_FUNC vec4<T> yyxz() const noexcept
    {
        return vec4(y, y, x, z);
    }
    CONST_FUNC vec4<T> zyxz() const noexcept
    {
        return vec4(z, y, x, z);
    }
    CONST_FUNC vec4<T> wyxz() const noexcept
    {
        return vec4(w, y, x, z);
    }
    CONST_FUNC vec4<T> xzxz() const noexcept
    {
        return vec4(x, z, x, z);
    }
    CONST_FUNC vec4<T> yzxz() const noexcept
    {
        return vec4(y, z, x, z);
    }
    CONST_FUNC vec4<T> zzxz() const noexcept
    {
        return vec4(z, z, x, z);
    }
    CONST_FUNC vec4<T> wzxz() const noexcept
    {
        return vec4(w, z, x, z);
    }
    CONST_FUNC vec4<T> xwxz() const noexcept
    {
        return vec4(x, w, x, z);
    }
    CONST_FUNC vec4<T> ywxz() const noexcept
    {
        return vec4(y, w, x, z);
    }
    CONST_FUNC vec4<T> zwxz() const noexcept
    {
        return vec4(z, w, x, z);
    }
    CONST_FUNC vec4<T> wwxz() const noexcept
    {
        return vec4(w, w, x, z);
    }
    CONST_FUNC vec4<T> xxyz() const noexcept
    {
        return vec4(x, x, y, z);
    }
    CONST_FUNC vec4<T> yxyz() const noexcept
    {
        return vec4(y, x, y, z);
    }
    CONST_FUNC vec4<T> zxyz() const noexcept
    {
        return vec4(z, x, y, z);
    }
    CONST_FUNC vec4<T> wxyz() const noexcept
    {
        return vec4(w, x, y, z);
    }
    CONST_FUNC vec4<T> xyyz() const noexcept
    {
        return vec4(x, y, y, z);
    }
    CONST_FUNC vec4<T> yyyz() const noexcept
    {
        return vec4(y, y, y, z);
    }
    CONST_FUNC vec4<T> zyyz() const noexcept
    {
        return vec4(z, y, y, z);
    }
    CONST_FUNC vec4<T> wyyz() const noexcept
    {
        return vec4(w, y, y, z);
    }
    CONST_FUNC vec4<T> xzyz() const noexcept
    {
        return vec4(x, z, y, z);
    }
    CONST_FUNC vec4<T> yzyz() const noexcept
    {
        return vec4(y, z, y, z);
    }
    CONST_FUNC vec4<T> zzyz() const noexcept
    {
        return vec4(z, z, y, z);
    }
    CONST_FUNC vec4<T> wzyz() const noexcept
    {
        return vec4(w, z, y, z);
    }
    CONST_FUNC vec4<T> xwyz() const noexcept
    {
        return vec4(x, w, y, z);
    }
    CONST_FUNC vec4<T> ywyz() const noexcept
    {
        return vec4(y, w, y, z);
    }
    CONST_FUNC vec4<T> zwyz() const noexcept
    {
        return vec4(z, w, y, z);
    }
    CONST_FUNC vec4<T> wwyz() const noexcept
    {
        return vec4(w, w, y, z);
    }
    CONST_FUNC vec4<T> xxzz() const noexcept
    {
        return vec4(x, x, z, z);
    }
    CONST_FUNC vec4<T> yxzz() const noexcept
    {
        return vec4(y, x, z, z);
    }
    CONST_FUNC vec4<T> zxzz() const noexcept
    {
        return vec4(z, x, z, z);
    }
    CONST_FUNC vec4<T> wxzz() const noexcept
    {
        return vec4(w, x, z, z);
    }
    CONST_FUNC vec4<T> xyzz() const noexcept
    {
        return vec4(x, y, z, z);
    }
    CONST_FUNC vec4<T> yyzz() const noexcept
    {
        return vec4(y, y, z, z);
    }
    CONST_FUNC vec4<T> zyzz() const noexcept
    {
        return vec4(z, y, z, z);
    }
    CONST_FUNC vec4<T> wyzz() const noexcept
    {
        return vec4(w, y, z, z);
    }
    CONST_FUNC vec4<T> xzzz() const noexcept
    {
        return vec4(x, z, z, z);
    }
    CONST_FUNC vec4<T> yzzz() const noexcept
    {
        return vec4(y, z, z, z);
    }
    CONST_FUNC vec4<T> zzzz() const noexcept
    {
        return vec4(z, z, z, z);
    }
    CONST_FUNC vec4<T> wzzz() const noexcept
    {
        return vec4(w, z, z, z);
    }
    CONST_FUNC vec4<T> xwzz() const noexcept
    {
        return vec4(x, w, z, z);
    }
    CONST_FUNC vec4<T> ywzz() const noexcept
    {
        return vec4(y, w, z, z);
    }
    CONST_FUNC vec4<T> zwzz() const noexcept
    {
        return vec4(z, w, z, z);
    }
    CONST_FUNC vec4<T> wwzz() const noexcept
    {
        return vec4(w, w, z, z);
    }
    CONST_FUNC vec4<T> xxwz() const noexcept
    {
        return vec4(x, x, w, z);
    }
    CONST_FUNC vec4<T> yxwz() const noexcept
    {
        return vec4(y, x, w, z);
    }
    CONST_FUNC vec4<T> zxwz() const noexcept
    {
        return vec4(z, x, w, z);
    }
    CONST_FUNC vec4<T> wxwz() const noexcept
    {
        return vec4(w, x, w, z);
    }
    CONST_FUNC vec4<T> xywz() const noexcept
    {
        return vec4(x, y, w, z);
    }
    CONST_FUNC vec4<T> yywz() const noexcept
    {
        return vec4(y, y, w, z);
    }
    CONST_FUNC vec4<T> zywz() const noexcept
    {
        return vec4(z, y, w, z);
    }
    CONST_FUNC vec4<T> wywz() const noexcept
    {
        return vec4(w, y, w, z);
    }
    CONST_FUNC vec4<T> xzwz() const noexcept
    {
        return vec4(x, z, w, z);
    }
    CONST_FUNC vec4<T> yzwz() const noexcept
    {
        return vec4(y, z, w, z);
    }
    CONST_FUNC vec4<T> zzwz() const noexcept
    {
        return vec4(z, z, w, z);
    }
    CONST_FUNC vec4<T> wzwz() const noexcept
    {
        return vec4(w, z, w, z);
    }
    CONST_FUNC vec4<T> xwwz() const noexcept
    {
        return vec4(x, w, w, z);
    }
    CONST_FUNC vec4<T> ywwz() const noexcept
    {
        return vec4(y, w, w, z);
    }
    CONST_FUNC vec4<T> zwwz() const noexcept
    {
        return vec4(z, w, w, z);
    }
    CONST_FUNC vec4<T> wwwz() const noexcept
    {
        return vec4(w, w, w, z);
    }
    CONST_FUNC vec4<T> xxxw() const noexcept
    {
        return vec4(x, x, x, w);
    }
    CONST_FUNC vec4<T> yxxw() const noexcept
    {
        return vec4(y, x, x, w);
    }
    CONST_FUNC vec4<T> zxxw() const noexcept
    {
        return vec4(z, x, x, w);
    }
    CONST_FUNC vec4<T> wxxw() const noexcept
    {
        return vec4(w, x, x, w);
    }
    CONST_FUNC vec4<T> xyxw() const noexcept
    {
        return vec4(x, y, x, w);
    }
    CONST_FUNC vec4<T> yyxw() const noexcept
    {
        return vec4(y, y, x, w);
    }
    CONST_FUNC vec4<T> zyxw() const noexcept
    {
        return vec4(z, y, x, w);
    }
    CONST_FUNC vec4<T> wyxw() const noexcept
    {
        return vec4(w, y, x, w);
    }
    CONST_FUNC vec4<T> xzxw() const noexcept
    {
        return vec4(x, z, x, w);
    }
    CONST_FUNC vec4<T> yzxw() const noexcept
    {
        return vec4(y, z, x, w);
    }
    CONST_FUNC vec4<T> zzxw() const noexcept
    {
        return vec4(z, z, x, w);
    }
    CONST_FUNC vec4<T> wzxw() const noexcept
    {
        return vec4(w, z, x, w);
    }
    CONST_FUNC vec4<T> xwxw() const noexcept
    {
        return vec4(x, w, x, w);
    }
    CONST_FUNC vec4<T> ywxw() const noexcept
    {
        return vec4(y, w, x, w);
    }
    CONST_FUNC vec4<T> zwxw() const noexcept
    {
        return vec4(z, w, x, w);
    }
    CONST_FUNC vec4<T> wwxw() const noexcept
    {
        return vec4(w, w, x, w);
    }
    CONST_FUNC vec4<T> xxyw() const noexcept
    {
        return vec4(x, x, y, w);
    }
    CONST_FUNC vec4<T> yxyw() const noexcept
    {
        return vec4(y, x, y, w);
    }
    CONST_FUNC vec4<T> zxyw() const noexcept
    {
        return vec4(z, x, y, w);
    }
    CONST_FUNC vec4<T> wxyw() const noexcept
    {
        return vec4(w, x, y, w);
    }
    CONST_FUNC vec4<T> xyyw() const noexcept
    {
        return vec4(x, y, y, w);
    }
    CONST_FUNC vec4<T> yyyw() const noexcept
    {
        return vec4(y, y, y, w);
    }
    CONST_FUNC vec4<T> zyyw() const noexcept
    {
        return vec4(z, y, y, w);
    }
    CONST_FUNC vec4<T> wyyw() const noexcept
    {
        return vec4(w, y, y, w);
    }
    CONST_FUNC vec4<T> xzyw() const noexcept
    {
        return vec4(x, z, y, w);
    }
    CONST_FUNC vec4<T> yzyw() const noexcept
    {
        return vec4(y, z, y, w);
    }
    CONST_FUNC vec4<T> zzyw() const noexcept
    {
        return vec4(z, z, y, w);
    }
    CONST_FUNC vec4<T> wzyw() const noexcept
    {
        return vec4(w, z, y, w);
    }
    CONST_FUNC vec4<T> xwyw() const noexcept
    {
        return vec4(x, w, y, w);
    }
    CONST_FUNC vec4<T> ywyw() const noexcept
    {
        return vec4(y, w, y, w);
    }
    CONST_FUNC vec4<T> zwyw() const noexcept
    {
        return vec4(z, w, y, w);
    }
    CONST_FUNC vec4<T> wwyw() const noexcept
    {
        return vec4(w, w, y, w);
    }
    CONST_FUNC vec4<T> xxzw() const noexcept
    {
        return vec4(x, x, z, w);
    }
    CONST_FUNC vec4<T> yxzw() const noexcept
    {
        return vec4(y, x, z, w);
    }
    CONST_FUNC vec4<T> zxzw() const noexcept
    {
        return vec4(z, x, z, w);
    }
    CONST_FUNC vec4<T> wxzw() const noexcept
    {
        return vec4(w, x, z, w);
    }
    CONST_FUNC vec4<T> xyzw() const noexcept
    {
        return vec4(x, y, z, w);
    }
    CONST_FUNC vec4<T> yyzw() const noexcept
    {
        return vec4(y, y, z, w);
    }
    CONST_FUNC vec4<T> zyzw() const noexcept
    {
        return vec4(z, y, z, w);
    }
    CONST_FUNC vec4<T> wyzw() const noexcept
    {
        return vec4(w, y, z, w);
    }
    CONST_FUNC vec4<T> xzzw() const noexcept
    {
        return vec4(x, z, z, w);
    }
    CONST_FUNC vec4<T> yzzw() const noexcept
    {
        return vec4(y, z, z, w);
    }
    CONST_FUNC vec4<T> zzzw() const noexcept
    {
        return vec4(z, z, z, w);
    }
    CONST_FUNC vec4<T> wzzw() const noexcept
    {
        return vec4(w, z, z, w);
    }
    CONST_FUNC vec4<T> xwzw() const noexcept
    {
        return vec4(x, w, z, w);
    }
    CONST_FUNC vec4<T> ywzw() const noexcept
    {
        return vec4(y, w, z, w);
    }
    CONST_FUNC vec4<T> zwzw() const noexcept
    {
        return vec4(z, w, z, w);
    }
    CONST_FUNC vec4<T> wwzw() const noexcept
    {
        return vec4(w, w, z, w);
    }
    CONST_FUNC vec4<T> xxww() const noexcept
    {
        return vec4(x, x, w, w);
    }
    CONST_FUNC vec4<T> yxww() const noexcept
    {
        return vec4(y, x, w, w);
    }
    CONST_FUNC vec4<T> zxww() const noexcept
    {
        return vec4(z, x, w, w);
    }
    CONST_FUNC vec4<T> wxww() const noexcept
    {
        return vec4(w, x, w, w);
    }
    CONST_FUNC vec4<T> xyww() const noexcept
    {
        return vec4(x, y, w, w);
    }
    CONST_FUNC vec4<T> yyww() const noexcept
    {
        return vec4(y, y, w, w);
    }
    CONST_FUNC vec4<T> zyww() const noexcept
    {
        return vec4(z, y, w, w);
    }
    CONST_FUNC vec4<T> wyww() const noexcept
    {
        return vec4(w, y, w, w);
    }
    CONST_FUNC vec4<T> xzww() const noexcept
    {
        return vec4(x, z, w, w);
    }
    CONST_FUNC vec4<T> yzww() const noexcept
    {
        return vec4(y, z, w, w);
    }
    CONST_FUNC vec4<T> zzww() const noexcept
    {
        return vec4(z, z, w, w);
    }
    CONST_FUNC vec4<T> wzww() const noexcept
    {
        return vec4(w, z, w, w);
    }
    CONST_FUNC vec4<T> xwww() const noexcept
    {
        return vec4(x, w, w, w);
    }
    CONST_FUNC vec4<T> ywww() const noexcept
    {
        return vec4(y, w, w, w);
    }
    CONST_FUNC vec4<T> zwww() const noexcept
    {
        return vec4(z, w, w, w);
    }
    CONST_FUNC vec4<T> wwww() const noexcept
    {
        return vec4(w, w, w, w);
    }


    /******** inplace geometry operations ********/

    CONST_FUNC vec4 & normalize() noexcept
    {
        static_assert(std::is_floating_point_v<T>, "`normalize` must done in fp vectors.");
        T n = std::sqrt(x * x + y * y + z * z + w * w);
        (*this) /= n;
        return *this;
    }
    // normalize vector and return this length before normalized.
    CONST_FUNC T normlen() noexcept
    {
        static_assert(std::is_floating_point_v<T>, "`normlen` must done in fp vectors.");
        T n = std::sqrt(x * x + y * y + z * z + w * w);
        (*this) /= n;
        return n;
    }

    template<class U> CONST_FUNC vec4 & reflect(vec4<U> const& n) noexcept
    {
        auto a = x * n.x + y * n.y + z * n.z + w * n.w;
        constexpr decltype(a) two = static_cast<decltype(a)>(2);
        x -= two * a * n.x; y -= two * a * n.y; z -= two * a * n.z; w -= two * a * n.w;
        return *this;
    }

    /******** inplace numerical operations ********/

    CONST_FUNC vec4 & neg() noexcept
    {
        if constexpr (std::is_same_v<T, bool>)
        {
            x = !x; y = !y; z = !z; w = !w;
        }
        else
        {
            x = -x; y = -y; z = -z; w = -w;
        }
        return *this;
    }

    template<class U> CONST_FUNC vec4 & operator += (U const& s) noexcept
    {
        x += s; y += s; z += s; w += s;
        return *this;
    }
    template<class U> CONST_FUNC vec4 & operator -= (U const& s) noexcept
    {
        x -= s; y -= s; z -= s; w -= s;
        return *this;
    }
    template<class U> CONST_FUNC vec4 & operator *= (U const& s) noexcept
    {
        x *= s; y *= s; z *= s; w *= s;
        return *this;
    }
    template<class U> CONST_FUNC vec4 & operator /= (U const& s) noexcept
    {
        x /= s; y /= s; z /= s; w /= s;
        return *this;
    }
    template<class U> CONST_FUNC vec4 & operator %= (U const& s) noexcept
    {
        x %= s; y %= s; z %= s; w %= s;
        return *this;
    }
    template<class U> CONST_FUNC vec4 & operator |= (U const& s) noexcept
    {
        x |= s; y |= s; z |= s; w |= s;
        return *this;
    }
    template<class U> CONST_FUNC vec4 & operator &= (U const& s) noexcept
    {
        x &= s; y &= s; z &= s; w &= s;
        return *this;
    }
    template<class U> CONST_FUNC vec4 & operator ^= (U const& s) noexcept
    {
        x ^= s; y ^= s; z ^= s; w ^= s;
        return *this;
    }
    template<class U> CONST_FUNC vec4 & operator <<=(U const& s) noexcept
    {
        x <<= s; y <<= s; z <<= s; w <<= s;
        return *this;
    }
    template<class U> CONST_FUNC vec4 & operator >>=(U const& s) noexcept
    {
        x >>= s; y >>= s; z >>= s; w >>= s;
        return *this;
    }

    template<class U> CONST_FUNC vec4 & operator += (vec4<U> const& v) noexcept
    {
        x += v.x; y += v.y; z += v.z; w += v.w;
        return *this;
    }
    template<class U> CONST_FUNC vec4 & operator -= (vec4<U> const& v) noexcept
    {
        x -= v.x; y -= v.y; z -= v.z; w -= v.w;
        return *this;
    }
    template<class U> CONST_FUNC vec4 & operator *= (vec4<U> const& v) noexcept
    {
        x *= v.x; y *= v.y; z *= v.z; w *= v.w;
        return *this;
    }
    template<class U> CONST_FUNC vec4 & operator /= (vec4<U> const& v) noexcept
    {
        x /= v.x; y /= v.y; z /= v.z; w /= v.w;
        return *this;
    }
    template<class U> CONST_FUNC vec4 & operator %= (vec4<U> const& v) noexcept
    {
        x %= v.x; y %= v.y; z %= v.z; w %= v.w;
        return *this;
    }
    template<class U> CONST_FUNC vec4 & operator |= (vec4<U> const& v) noexcept
    {
        x |= v.x; y |= v.y; z |= v.z; w |= v.w;
        return *this;
    }
    template<class U> CONST_FUNC vec4 & operator &= (vec4<U> const& v) noexcept
    {
        x &= v.x; y &= v.y; z &= v.z; w &= v.w;
        return *this;
    }
    template<class U> CONST_FUNC vec4 & operator ^= (vec4<U> const& v) noexcept
    {
        x ^= v.x; y ^= v.y; z ^= v.z; w ^= v.w;
        return *this;
    }
    template<class U> CONST_FUNC vec4 & operator <<=(vec4<U> const& v) noexcept
    {
        x <<= v.x; y <<= v.y; z <<= v.z; w <<= v.w;
        return *this;
    }
    template<class U> CONST_FUNC vec4 & operator >>=(vec4<U> const& v) noexcept
    {
        x >>= v.x; y >>= v.y; z >>= v.z; w >>= v.w;
        return *this;
    }

    template<class U> CONST_FUNC vec4 & add(U const& other) noexcept
    {
        (*this) += other;
        return *this;
    }
    template<class U> CONST_FUNC vec4 & sub(U const& other) noexcept
    {
        (*this) -= other;
        return *this;
    }
    template<class U> CONST_FUNC vec4 & mul(U const& other) noexcept
    {
        (*this) *= other;
        return *this;
    }
    template<class U> CONST_FUNC vec4 & div(U const& other) noexcept
    {
        (*this) /= other;
        return *this;
    }
    template<class U> CONST_FUNC vec4 & mod(U const& other) noexcept
    {
        (*this) &= other;
        return *this;
    }

    /******** compare each elements operations ********/

    template <class U> CONST_FUNC vec4<bool> isless(U const& s) const noexcept
    {
        return vec4(x < s, y < s, z < s, w < s);
    }
    template <class U> CONST_FUNC vec4<bool> isleq(U const& s) const noexcept
    {
        return vec4(x <= s, y <= s, z <= s, w <= s);
    }
    template <class U> CONST_FUNC vec4<bool> iseq(U const& s) const noexcept
    {
        return vec4(x == s, y == s, z == s, w == s);
    }
    template <class U> CONST_FUNC vec4<bool> isneq(U const& s) const noexcept
    {
        return vec4(x != s, y != s, z != s, w != s);
    }
    template <class U> CONST_FUNC vec4<bool> isgeq(U const& s) const noexcept
    {
        return vec4(x >= s, y >= s, z >= s, w >= s);
    }
    template <class U> CONST_FUNC vec4<bool> isgreater(U const& s) const noexcept
    {
        return vec4(x > s, y > s, z > s, w > s);
    }

    template <class U> CONST_FUNC vec4<bool> isless(vec4<U> const& v) const noexcept
    {
        return vec4(x < v.x, y < v.y, z < v.z, w < v.w);
    }
    template <class U> CONST_FUNC vec4<bool> isleq(vec4<U> const& v) const noexcept
    {
        return vec4(x <= v.x, y <= v.y, z <= v.z, w <= v.w);
    }
    template <class U> CONST_FUNC vec4<bool> iseq(vec4<U> const& v) const noexcept
    {
        return vec4(x == v.x, y == v.y, z == v.z, w == v.w);
    }
    template <class U> CONST_FUNC vec4<bool> isneq(vec4<U> const& v) const noexcept
    {
        return vec4(x != v.x, y != v.y, z != v.z, w != v.w);
    }
    template <class U> CONST_FUNC vec4<bool> isgeq(vec4<U> const& v) const noexcept
    {
        return vec4(x >= v.x, y >= v.y, z >= v.z, w >= v.w);
    }
    template <class U> CONST_FUNC vec4<bool> isgreater(vec4<U> const& v) const noexcept
    {
        return vec4(x > v.x, y > v.y, z > v.z, w > v.w);
    }
};

// the vector type used in geometry calculations
using vec4g = vec4<fg>;


/********** default values **********/

template<class T> class defaults<vec4<T>>
{
public:

    static constexpr vec4<T> O = vec4<T>(static_cast<T>(0), static_cast<T>(0), static_cast<T>(0), static_cast<T>(0));
    static constexpr vec4<T> X = vec4<T>(static_cast<T>(1), static_cast<T>(0), static_cast<T>(0), static_cast<T>(0));
    static constexpr vec4<T> Y = vec4<T>(static_cast<T>(0), static_cast<T>(1), static_cast<T>(0), static_cast<T>(0));
    static constexpr vec4<T> Z = vec4<T>(static_cast<T>(0), static_cast<T>(0), static_cast<T>(1), static_cast<T>(0));
    static constexpr vec4<T> W = vec4<T>(static_cast<T>(0), static_cast<T>(0), static_cast<T>(0), static_cast<T>(1));
    static constexpr vec4<T> min = vec4<T>(std::numeric_limits<T>::min());
    static constexpr vec4<T> max = vec4<T>(std::numeric_limits<T>::max());
};


/********** geometry operations **********/

template<class T, class U> CONST_FUNC std::common_type_t<T, U> dot(vec4<T> const& v1, vec4<U> const& v2) noexcept
{
    return v1.x * v2.x + v1.y * v2.y + v1.z * v2.z + v1.w * v2.w;
}

template<class T> CONST_FUNC T length2(vec4<T> const& v) noexcept
{
    return dot(v, v);
}
template<class T> CONST_FUNC T length(vec4<T> const& v) noexcept
{
    static_assert(std::is_floating_point_v<T>, "`length` must done in fp vectors.");
    return std::sqrt(length2(v));
}

template<class T> CONST_FUNC vec4<T> normalize(vec4<T> const& v) noexcept
{
    static_assert(std::is_floating_point_v<T>, "`normalize` must done in fp vectors.");
    T n = length(v);
    return vec4(v.x / n, v.y / n, v.z / n, v.w / n);
}
// normalized vector and the length of input vector
template<class T> CONST_FUNC std::tuple<vec4<T>, T> normlen(vec4<T> const& v) noexcept
{
    static_assert(std::is_floating_point_v<T>, "`normlen` must done in fp vectors.");
    T n = length(v);
    return {vec4(v.x / n, v.y / n, v.z / n, v.w / n), n};
}

template<class T, class U> vec4<std::common_type_t<T, U>> reflect(vec4<T> const& v, vec4<U> const& n) noexcept
{
    return v - static_cast<std::common_type_t<T, U>>(2) * dot(v, n) * n;
}

template<class T, class U> vec4<std::common_type_t<T, U>> qmul(vec4<T> const& l, vec4<U> const& r) noexcept
{
    return vec4(
        l.w * r.w - l.x * r.x - l.y * r.y - l.z * r.z,
        l.w * r.x + l.x * r.w + l.y * r.z - l.z * r.y,
        l.w * r.y - l.x * r.z + l.y * r.w + l.z * r.x,
        l.w * r.z + l.x * r.y - l.y * r.x + l.z * r.w
    );
}
template<class T, class U> vec4<std::common_type_t<T, U>> rotation(vec3<T> const& rotation_axis, U const& rotate_angle) noexcept
{
    using R = std::common_type_t<T, U>;
    return vec4(std::sin(static_cast<R>(rotate_angle) / 2) * normalize(rotation_axis), std::cos(static_cast<R>(rotate_angle) / 2));
}
template<class T, class U> vec3<std::common_type_t<T, U>> rotate(vec4<T> const& r, vec3<U> const& v) noexcept
{
    T xx = r.x*r.x, yy = r.y*r.y, zz = r.z*r.z, ww = r.w*r.w;
    T xy = r.x*r.y, xz = r.x*r.z, xw = r.x*r.w;
    T yz = r.y*r.z, yw = r.y*r.w;
    T zw = r.z*r.w;
    return vec3(
        v.x * (yy + zz - xx - ww) + v.y * (2 * (zw - xy)) + v.z * (2 * (-yw - xz)),
        v.x * (2 * (-zw - xy)) + v.y * (xx - yy + zz - ww) + v.z * (2 * (xw - yz)),
        v.x * (2 * (yw - xz)) + v.y * (2 * (-xw - yz)) + v.z * (xx + yy - zz - ww)
    );
}

/******** numerical operations ********/
// TODO: compare each elements and return a vector of booleans

template<class T> CONST_FUNC T sum(vec4<T> const& v) noexcept
{
    return v.x + v.y + v.z + v.w;
}

template<class T> CONST_FUNC vec4<T> operator + (vec4<T> const& v) noexcept
{
    return vec4(+v.x, +v.y, +v.z, +v.w);
}
template<class T> CONST_FUNC vec4<T> operator - (vec4<T> const& v) noexcept
{
    return vec4(-v.x, -v.y, -v.z, -v.w);
}
template<class T> CONST_FUNC vec4<T> operator ~ (vec4<T> const& v) noexcept
{
    return vec4(~v.x, ~v.y, ~v.z, ~v.w);
}
CONST_FUNC vec4<bool> operator ! (vec4<bool> const& v) noexcept
{
    return vec4(!v.x, !v.y, !v.z, !v.w);
}

CONST_FUNC bool all(vec4<bool> const& v) noexcept
{
    return v.x && v.y && v.z && v.w;
}
CONST_FUNC bool any(vec4<bool> const& v) noexcept
{
    return v.x || v.y || v.z || v.w;
}
CONST_FUNC bool not_all_equal(vec4<bool> const& v) noexcept
{
    return ((!v.x) != (!v.y)) || ((!v.y) != (!v.z)) || ((!v.z) != (!v.w));
}

template<class T, class U> CONST_FUNC vec4<std::common_type_t<T, U>> operator + (vec4<T> const& v, U const& s) noexcept
{
    return vec4(v.x + s, v.y + s, v.z + s, v.w + s);
}
template<class T, class U> CONST_FUNC vec4<std::common_type_t<T, U>> operator - (vec4<T> const& v, U const& s) noexcept
{
    return vec4(v.x - s, v.y - s, v.z - s, v.w - s);
}
template<class T, class U> CONST_FUNC vec4<std::common_type_t<T, U>> operator * (vec4<T> const& v, U const& s) noexcept
{
    return vec4(v.x * s, v.y * s, v.z * s, v.w * s);
}
template<class T, class U> CONST_FUNC vec4<std::common_type_t<T, U>> operator / (vec4<T> const& v, U const& s) noexcept
{
    return vec4(v.x / s, v.y / s, v.z / s, v.w / s);
}
template<class T, class U> CONST_FUNC vec4<std::common_type_t<T, U>> operator % (vec4<T> const& v, U const& s) noexcept
{
    return vec4(v.x % s, v.y % s, v.z % s, v.w % s);
}
template<class T, class U> CONST_FUNC vec4<std::common_type_t<T, U>> operator | (vec4<T> const& v, U const& s) noexcept
{
    return vec4(v.x | s, v.y | s, v.z | s, v.w | s);
}
template<class T, class U> CONST_FUNC vec4<std::common_type_t<T, U>> operator & (vec4<T> const& v, U const& s) noexcept
{
    return vec4(v.x & s, v.y & s, v.z & s, v.w & s);
}
template<class T, class U> CONST_FUNC vec4<std::common_type_t<T, U>> operator ^ (vec4<T> const& v, U const& s) noexcept
{
    return vec4(v.x ^ s, v.y ^ s, v.z ^ s, v.w ^ s);
}
template<class T, class U> CONST_FUNC vec4<std::common_type_t<T, U>> operator <<(vec4<T> const& v, U const& s) noexcept
{
    return vec4(v.x << s, v.y << s, v.z << s, v.w << s);
}
template<class T, class U> CONST_FUNC vec4<std::common_type_t<T, U>> operator >>(vec4<T> const& v, U const& s) noexcept
{
    return vec4(v.x >> s, v.y >> s, v.z >> s, v.w >> s);
}

template<class T, class U> CONST_FUNC vec4<std::common_type_t<T, U>> operator + (T const& s, vec4<U> const& v) noexcept
{
    return vec4(s + v.x, s + v.y, s + v.z, s + v.w);
}
template<class T, class U> CONST_FUNC vec4<std::common_type_t<T, U>> operator - (T const& s, vec4<U> const& v) noexcept
{
    return vec4(s - v.x, s - v.y, s - v.z, s - v.w);
}
template<class T, class U> CONST_FUNC vec4<std::common_type_t<T, U>> operator * (T const& s, vec4<U> const& v) noexcept
{
    return vec4(s * v.x, s * v.y, s * v.z, s * v.w);
}
template<class T, class U> CONST_FUNC vec4<std::common_type_t<T, U>> operator / (T const& s, vec4<U> const& v) noexcept
{
    return vec4(s / v.x, s / v.y, s / v.z, s / v.w);
}
template<class T, class U> CONST_FUNC vec4<std::common_type_t<T, U>> operator % (T const& s, vec4<U> const& v) noexcept
{
    return vec4(s % v.x, s % v.y, s % v.z, s % v.w);
}
template<class T, class U> CONST_FUNC vec4<std::common_type_t<T, U>> operator | (T const& s, vec4<U> const& v) noexcept
{
    return vec4(s | v.x, s | v.y, s | v.z, s | v.w);
}
template<class T, class U> CONST_FUNC vec4<std::common_type_t<T, U>> operator & (T const& s, vec4<U> const& v) noexcept
{
    return vec4(s & v.x, s & v.y, s & v.z, s & v.w);
}
template<class T, class U> CONST_FUNC vec4<std::common_type_t<T, U>> operator ^ (T const& s, vec4<U> const& v) noexcept
{
    return vec4(s ^ v.x, s ^ v.y, s ^ v.z, s ^ v.w);
}
template<class T, class U> CONST_FUNC vec4<std::common_type_t<T, U>> operator <<(T const& s, vec4<U> const& v) noexcept
{
    return vec4(s << v.x, s << v.y, s << v.z, s << s.w);
}
template<class T, class U> CONST_FUNC vec4<std::common_type_t<T, U>> operator >>(T const& s, vec4<U> const& v) noexcept
{
    return vec4(s >> v.x, s >> v.y, s >> v.z, s >> s.w);
}

template<class T, class U> CONST_FUNC vec4<std::common_type_t<T, U>> operator + (vec4<T> const& v1, vec4<U> const& v2) noexcept
{
    return vec4(v1.x + v2.x, v1.y + v2.y, v1.z + v2.z, v1.w + v2.w);
}
template<class T, class U> CONST_FUNC vec4<std::common_type_t<T, U>> operator - (vec4<T> const& v1, vec4<U> const& v2) noexcept
{
    return vec4(v1.x - v2.x, v1.y - v2.y, v1.z - v2.z, v1.w - v2.w);
}
template<class T, class U> CONST_FUNC vec4<std::common_type_t<T, U>> operator * (vec4<T> const& v1, vec4<U> const& v2) noexcept
{
    return vec4(v1.x * v2.x, v1.y * v2.y, v1.z * v2.z, v1.w * v2.w);
}
template<class T, class U> CONST_FUNC vec4<std::common_type_t<T, U>> operator / (vec4<T> const& v1, vec4<U> const& v2) noexcept
{
    return vec4(v1.x / v2.x, v1.y / v2.y, v1.z / v2.z, v1.w / v2.w);
}
template<class T, class U> CONST_FUNC vec4<std::common_type_t<T, U>> operator % (vec4<T> const& v1, vec4<U> const& v2) noexcept
{
    return vec4(v1.x % v2.x, v1.y % v2.y, v1.z % v2.z, v1.w % v2.w);
}
template<class T, class U> CONST_FUNC vec4<std::common_type_t<T, U>> operator | (vec4<T> const& v1, vec4<U> const& v2) noexcept
{
    return vec4(v1.x | v2.x, v1.y | v2.y, v1.z | v2.z, v1.w | v2.w);
}
template<class T, class U> CONST_FUNC vec4<std::common_type_t<T, U>> operator & (vec4<T> const& v1, vec4<U> const& v2) noexcept
{
    return vec4(v1.x & v2.x, v1.y & v2.y, v1.z & v2.z, v1.w & v2.w);
}
template<class T, class U> CONST_FUNC vec4<std::common_type_t<T, U>> operator ^ (vec4<T> const& v1, vec4<U> const& v2) noexcept
{
    return vec4(v1.x ^ v2.x, v1.y ^ v2.y, v1.z ^ v2.z, v1.w ^ v2.w);
}
template<class T, class U> CONST_FUNC vec4<std::common_type_t<T, U>> operator <<(vec4<T> const& v1, vec4<U> const& v2) noexcept
{
    return vec4(v1.x << v2.x, v1.y << v2.y, v1.z << v2.z, v1.w << v2.w);
}
template<class T, class U> CONST_FUNC vec4<std::common_type_t<T, U>> operator >>(vec4<T> const& v1, vec4<U> const& v2) noexcept
{
    return vec4(v1.x >> v2.x, v1.y >> v2.y, v1.z >> v2.z, v1.w >> v2.w);
}
