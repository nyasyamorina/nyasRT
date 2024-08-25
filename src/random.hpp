#pragma once

#include <bit>
#include <chrono>
#include <math.h>
#include <numeric>
#include <thread>

#include "utils.hpp"
#include "geometry/vec2.hpp"
#include "geometry/vec3.hpp"
#include "geometry/vec4.hpp"
#include "RGB.hpp"


// keep the period still 2^64, instead of reducing it to 2^32 when double calling
constexpr bool KEEP_PERIOD_LONG_AS_POSSIBLE = false;



template<class T> class _Generation;

class PCG_XSH_RR_32 final
{
public:

    static constexpr u64 multiplier = 0x5851F42D4C957F2D;

    u64 _increment, _state;

    static u64 _default_seeding() noexcept
    {
        using namespace std::chrono;

        u64 seed = static_cast<u64>(std::hash<std::thread::id>{}(std::this_thread::get_id()));
        seed *= duration_cast<nanoseconds>(system_clock::now().time_since_epoch()).count();
        return seed;
    }

public:

    PCG_XSH_RR_32() noexcept
    : PCG_XSH_RR_32(_default_seeding()) {}
    CONST_FUNC PCG_XSH_RR_32(u64 seed) noexcept
    : _increment((seed << 1) | 1), _state(seed) {}

    CONST_FUNC void next() noexcept
    {
        _state *= multiplier;
        _state += _increment;
    }
    CONST_FUNC u32 get() noexcept
    {
        next();
        return std::rotr(static_cast<u32>((_state ^ (_state >> 18)) >> 27), static_cast<i32>(_state >> 59));
    }

    template<class T> CONST_FUNC T randbits() noexcept
    {
        return _Generation<T>::randbits(*this);
    }
    template<class T> CONST_FUNC T uniform01() noexcept
    {
        return _Generation<T>::uniform01(*this);
    }
};

static thread_local PCG_XSH_RR_32 random;


inline void _keep_period_or_not(PCG_XSH_RR_32 & rng) noexcept
{
    if constexpr (KEEP_PERIOD_LONG_AS_POSSIBLE)
    {
        // keep the period still 2^64, instead of reducing it to 2^32 when double calling
        rng.next();
    }
}

template<class T> class _Generation<vec2<T>>
{
public:

    static CONST_FUNC vec2<T> randbits(PCG_XSH_RR_32 & rng) noexcept
    {
        _keep_period_or_not(rng);
        return vec2<T>(rng.randbits<T>(), rng.randbits<T>());
    }
    static CONST_FUNC vec2<T> uniform01(PCG_XSH_RR_32 & rng) noexcept
    {
        _keep_period_or_not(rng);
        return vec2<T>(rng.uniform01<T>(), rng.uniform01<T>());
    }
};
template<class T> class _Generation<vec3<T>>
{
public:

    static CONST_FUNC vec3<T> randbits(PCG_XSH_RR_32 & rng) noexcept
    {
        return vec3<T>(rng.randbits<T>(), rng.randbits<T>(), rng.randbits<T>());
    }
    static CONST_FUNC vec3<T> uniform01(PCG_XSH_RR_32 & rng) noexcept
    {
        return vec3<T>(rng.uniform01<T>(), rng.uniform01<T>(), rng.uniform01<T>());
    }
};
template<class T> class _Generation<vec4<T>>
{
public:

    static CONST_FUNC vec4<T> randbits(PCG_XSH_RR_32 & rng) noexcept
    {
        _keep_period_or_not(rng);
        return vec4<T>(rng.randbits<T>(), rng.randbits<T>(), rng.randbits<T>(), rng.randbits<T>());
    }
    static CONST_FUNC vec4<T> uniform01(PCG_XSH_RR_32 & rng) noexcept
    {
        _keep_period_or_not(rng);
        return vec4<T>(rng.uniform01<T>(), rng.uniform01<T>(), rng.uniform01<T>(), rng.uniform01<T>());
    }
};

template<> class _Generation<u32>
{
public:

    static CONST_FUNC u32 randbits(PCG_XSH_RR_32 & rng) noexcept
    {
        return rng.get();
    }
    static CONST_FUNC u32 uniform01(PCG_XSH_RR_32 &) noexcept
    {
        return 0;   // `uniform01` means output number in [0,1), the only integer in this range is 0.
    }
};
template<> class _Generation<u64>
{
public:

    static CONST_FUNC u64 randbits(PCG_XSH_RR_32 & rng) noexcept
    {
        _keep_period_or_not(rng);
        return (static_cast<u64>(rng.get()) << 32) | static_cast<u64>(rng.get());
    }
    static CONST_FUNC u64 uniform01(PCG_XSH_RR_32 &) noexcept
    {
        return 0;   // `uniform01` means output number in [0,1), the only integer in this range is 0.
    }
};

template<> class _Generation<f32>
{
public:

    static CONST_FUNC f32 randbits(PCG_XSH_RR_32 & rng) noexcept
    {
        return std::bit_cast<f32>(rng.randbits<u32>());
    }
    static CONST_FUNC f32 uniform01(PCG_XSH_RR_32 & rng) noexcept
    {
        constexpr f32 dividor = static_cast<f32>(std::numeric_limits<u32>::max()) + 1.0f;
        return static_cast<f32>(rng.randbits<u32>()) / dividor;
    }
};
template<> class _Generation<f64>
{
public:

    static CONST_FUNC f64 randbits(PCG_XSH_RR_32 & rng) noexcept
    {
        return std::bit_cast<f64>(rng.randbits<u64>());
    }
    static CONST_FUNC f64 uniform01(PCG_XSH_RR_32 & rng) noexcept
    {
        constexpr f64 dividor = static_cast<f64>(std::numeric_limits<u64>::max()) + 1.0;
        return static_cast<f64>(rng.randbits<u64>()) / dividor;
    }
};

template<> class _Generation<RGB>
{
public:

    static CONST_FUNC RGB randbits(PCG_XSH_RR_32 & rng) noexcept
    {
        return RGB(rng.randbits<f32>(), rng.randbits<f32>(), rng.randbits<f32>());
    }
    static CONST_FUNC RGB uniform01(PCG_XSH_RR_32 & rng) noexcept
    {
        return RGB(rng.uniform01<f32>(), rng.uniform01<f32>(), rng.uniform01<f32>());
    }
};
