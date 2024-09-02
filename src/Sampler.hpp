#pragma once

#include <vector>

#include "utils.hpp"
#include "geometry/vec2.hpp"
#include "geometry/vec3.hpp"
#include "random.hpp"


// multi-jittered sampler
class Sampler final
{
public:

    // map [0,1)^2 to unit disk(circle)
    static CONST_FUNC vec2g disk(vec2g uv) noexcept
    {
        uv.mul(2).sub(1);
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
        phi *= defaults<fg>::half * defaults<fg>::half_pi;
        return vec2g(r * std::cos(phi), r * std::sin(phi));
    }
    // map [0,1)^2 to unit sphere
    static CONST_FUNC normal3g sphere(vec2g const uv) noexcept
    {
        fg phi = 2 * defaults<fg>::two_pi * uv.x;
        fg sphi = std::sin(phi), cphi = std::cos(phi);
        fg ctheta = 1 - 2 * uv.y;
        fg stheta = std::sqrt(1 - sqr(ctheta));
        return vec3g(stheta * cphi, stheta * sphi, ctheta);
    }
    // map [0,1)^2 to unit upper hemisphere, the distribution on solid angle satisfies `dΩ = cosθ*dθdφ`
    static CONST_FUNC normal3g hemisphere(vec2g const uv) noexcept
    {
        fg phi = 2 * defaults<fg>::two_pi * uv.x;
        fg sphi = std::sin(phi), cphi = std::cos(phi);
        fg ctheta = std::sqrt(1 - uv.y);
        fg stheta = std::sqrt(1 - sqr(ctheta));
        return vec3g(stheta * cphi, stheta * sphi, ctheta);
    }

private:

    u32 _n_sample_sets, _n_samples_per_set /* always square number */;
    u32 _sample_index, _n_samples;
    std::vector<vec2g> _samples;

public:

    Sampler(u32 n_sample_sets_, u32 n_samples_per_set_)
    : _n_sample_sets(n_sample_sets_), _sample_index{0} {
        u32 side_len = std::sqrt(static_cast<f32>(n_samples_per_set_));
        _n_samples_per_set = sqr(side_len);
        _n_samples = n_sample_sets_ * _n_samples_per_set;

        fg cell_size = 1 / static_cast<fg>(side_len);

        _samples.reserve(_n_samples_per_set * n_sample_sets_);
        // generate samples in each set
        u32 set_start = 0;
        for (; n_sample_sets_ > 0; n_sample_sets_--)
        {
            // generate samples
            for (u32 y = 0; y < side_len; y++)
            {
                for (u32 x = 0; x < side_len; x++)
                {
                    _samples.push_back(((vec2g(y, x) + random.uniform01<vec2g>()) * cell_size + vec2g(x, y)) * cell_size);
                }
            }
            //shuffle x
            for (u32 y = 0; y < side_len; y++)
            {
                for (u32 x = 0; x < side_len; x++)
                {
                    u32 k = random.randbits<u32>() % (side_len - y);
                    std::swap(_samples[set_start + y * side_len + x].x, _samples[set_start + k * side_len + x].x);
                }
            }
            //shuffle y
            for (u32 x = 0; x < side_len; x++)
            {
                for (u32 y = 0; y < side_len; y++)
                {
                    u32 k = random.randbits<u32>() % (side_len - x);
                    std::swap(_samples[set_start + y * side_len + x].y, _samples[set_start + y * side_len + k].y);
                }
            }
            set_start += _n_samples_per_set;
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

    vec2g get_sample() noexcept
    {
        vec2g const& sample = _samples[_sample_index++];
        if (_sample_index >= _n_samples) { _sample_index = 0; }
        return sample;
    }
};

//TODO: static thread_local Sampler sampler(83, SAMPLE_SIZE);
