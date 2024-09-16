#pragma once

#include <vector>

#include "common.hpp"
#include "PCG.hpp"


namespace nyasRT
{
enum class SampleType : u8
{
    Random,
    MultiJittered,
};

template<SampleType sample> struct _SampleGenerator;
// _SampleGenerator<...>::generate_at(vec2g * samples, u32 n);


void generate_samples(SampleType sample_type, vec2g * samples, u32 n) noexcept;

class Sampler final
{
public:

    /******** map sample in unifrom square to ... ********/

    // map [0,1)^2 to unit disk(circle)
    static VEC_CONSTEXPR inline vec2g disk(vec2g uv) noexcept
    {
        uv = uv * fg(2) - fg(1);
        fg r, phi = 0;
        if (-uv.x > uv.y)
        {
            uv *= -1;
            phi = 4;
        }
        if (uv.x > uv.y)
        {
            r = uv.x;
            phi += uv.y / uv.x;
        }
        else
        {
            r = uv.y;
            if (uv.y != 0) { phi += 2 - uv.x / uv.y; }
        }
        phi *= consts<fg>::half * consts<fg>::half_pi;
        return vec2g(r * std::cos(phi), r * std::sin(phi));
    }
    // map [0,1)^2 to unit sphere
    static VEC_CONSTEXPR inline normal3g sphere(vec2g const uv) noexcept
    {
        fg phi = 2 * consts<fg>::two_pi * uv.x;
        fg sphi = std::sin(phi), cphi = std::cos(phi);
        fg ctheta = 1 - 2 * uv.y;
        fg stheta = std::sqrt(1 - sqr(ctheta));
        return vec3g(stheta * cphi, stheta * sphi, ctheta);
    }
    // map [0,1)^2 to unit upper hemisphere, the distribution on solid angle satisfies `dΩ = cosθ*dθdφ`
    static VEC_CONSTEXPR inline normal3g hemisphere(vec2g const uv) noexcept
    {
        fg phi = 2 * consts<fg>::two_pi * uv.x;
        fg sphi = std::sin(phi), cphi = std::cos(phi);
        fg ctheta = std::sqrt(1 - uv.y);
        fg stheta = std::sqrt(1 - sqr(ctheta));
        return vec3g(stheta * cphi, stheta * sphi, ctheta);
    }

private:

    u32 _n_sample_sets, _n_samples_per_set;
    u32 _sample_index, _n_samples;
    std::vector<vec2g> _samples;

public:

    Sampler() noexcept
    : _n_sample_sets(0), _n_samples_per_set{0}, _sample_index{0}, _n_samples{0}, _samples{} {}

    void init(SampleType sample_type, u32 n_sample_sets_, u32 n_samples_per_set_) noexcept
    {
        if ((n_sample_sets_ != _n_sample_sets) || (n_samples_per_set_ != _n_samples_per_set))
        {
            _n_sample_sets = n_sample_sets_;
            _n_samples_per_set = n_samples_per_set_;
            _sample_index = 0;
            _n_samples = n_sample_sets_ * n_samples_per_set_;
            _samples.resize(_n_samples);
        }

        vec2g * samples = _samples.data();
        for (u32 set_index = 0; set_index < _n_sample_sets; set_index++)
        {
            generate_samples(sample_type, samples, _n_samples_per_set);
            samples += _n_samples_per_set;
        }
    }

    u32 n_sample_set() const noexcept
    {
        return _n_sample_sets;
    }
    u32 n_samples_per_set() const noexcept
    {
        return _n_samples_per_set;
    }
    u32 n_samples() const noexcept
    {
        return _n_samples;
    }
    std::vector<vec2g> const& samples() const noexcept
    {
        return _samples;
    }

    vec2g get() noexcept
    {
        vec2g const& sample = _samples[_sample_index++];
        if (_sample_index >= _n_samples) { _sample_index = 0; }
        return sample;
    }
};

static thread_local Sampler sampler;


template<> struct _SampleGenerator<SampleType::Random>
{
    static void generate_at(vec2g * samples, u32 n) noexcept
    {
        for (; n > 0; n--)
        {
            *(samples++) = pcg.uniform01<vec2g>();
        }
    }
};
template<> struct _SampleGenerator<SampleType::MultiJittered>
{
    static void generate_at(vec2g * samples, u32 n) noexcept
    {
        if (n == 0) { return; }

        u32 side_len = std::sqrt(f32(n));
        u32 _n = sqr(side_len);

        fg cell_size = 1 / fg(side_len);

        // generate samples
        for (u32 y = 0; y < side_len; y++)
        {
            for (u32 x = 0; x < side_len; x++)
            {
                samples[y * side_len + x] = ((vec2g(y, x) + pcg.uniform01<vec2g>()) * cell_size + vec2g(x, y)) * cell_size;
            }
        }
        //shuffle x
        for (u32 y = 0; y < side_len; y++)
        {
            for (u32 x = 0; x < side_len; x++)
            {
                u32 k = pcg.randbits<u32>() % (side_len - y);
                std::swap(samples[y * side_len + x].x, samples[k * side_len + x].x);
            }
        }
        //shuffle y
        for (u32 x = 0; x < side_len; x++)
        {
            for (u32 y = 0; y < side_len; y++)
            {
                u32 k = pcg.randbits<u32>() % (side_len - x);
                std::swap(samples[y * side_len + x].x, samples[y * side_len + k].x);
            }
        }

        // generate the rests
        generate_at(samples + _n, n - _n);
    }
};

void inline generate_samples(SampleType sample_type, vec2g * samples, u32 n) noexcept
{
    switch (sample_type)
    {
    case SampleType::MultiJittered:
        return _SampleGenerator<SampleType::MultiJittered>::generate_at(samples, n);
    default:
        return _SampleGenerator<SampleType::Random>::generate_at(samples, n);
    }
}

} // namespace nyasRT
