#pragma once

#include <memory>

#include "../utils.hpp"
#include "../geometry/vec2.hpp"
#include "../geometry/Ray.hpp"


class Camera
{
public:

    virtual bool prepare()
    {
        return true;
    }

    virtual Ray cast_ray(vec2g const& screen_position /* in [-1,1] */) const noexcept = 0;
};

using CameraPtr = std::shared_ptr<Camera>;
using CameraConstPtr = std::shared_ptr<Camera const>;
