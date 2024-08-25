#pragma once

#include <memory>

#include "../utils.hpp"
#include "../geometry/vec3.hpp"
#include "../RGB.hpp"


class Sky
{
public:

    virtual bool prepare() noexcept
    {
        return true;
    }

    virtual RGB operator () (vec3g const& NORMALIZED direction) const noexcept = 0;
};

using SkyPtr = std::shared_ptr<Sky>;
using SkyConstPtr = std::shared_ptr<Sky const>;
