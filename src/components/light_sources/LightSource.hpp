#pragma once

#include <tuple>

#include "../../common.hpp"
#include "../../geometry/Ray.hpp"


namespace nyasRT
{
namespace light_sources
{
class LightSource
{
public:

    virtual bool prepare() noexcept
    {
        return true;
    }

    virtual bool test_hit(Ray const& ray, fg max_ray_time) const noexcept = 0;

    virtual RGB light(Ray const& ray) const noexcept = 0;

    virtual std::tuple<normal3g, fg> sample(vec3g const& point) const noexcept = 0;
};

} // namespace light_sources
} // namespace nyasRT
