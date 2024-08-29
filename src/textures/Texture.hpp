#pragma once

#include <memory>

#include "../utils.hpp"
#include "../geometry/vec2.hpp"
#include "../RGB.hpp"
#include "../geometry/Ray.hpp"


class Texture
{
public:

    virtual bool prepare() noexcept
    {
        return true;
    }

    virtual RGB operator () (Ray const& ray, TraceRecord const& rec) const noexcept = 0;
};

using TexturePtr = std::shared_ptr<Texture>;
using TextureConstPtr = std::shared_ptr<Texture const>;
