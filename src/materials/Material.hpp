#pragma once

#include <memory>

#include "../utils.hpp"
#include "../geometry/vec2.hpp"
#include "../RGB.hpp"
#include "../geometry/Ray.hpp"


class Material
{
public:

    virtual bool prepare() noexcept
    {
        return true;
    }

    virtual RGB operator () (Ray const& ray, TraceRecord const& rec) const noexcept = 0;
};

using MaterialPtr = std::shared_ptr<Material>;
using MaterialConstPtr = std::shared_ptr<Material const>;
