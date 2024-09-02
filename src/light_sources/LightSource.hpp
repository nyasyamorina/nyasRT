#pragma once

#include <memory>
#include <tuple>

#include "../utils.hpp"
#include "../geometry/vec3.hpp"
#include "../RGB.hpp"
#include "../geometry/Ray.hpp"


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

using LightSourcePtr = std::shared_ptr<LightSource>;
using LightSourceConstPtr = std::shared_ptr<LightSource const>;
