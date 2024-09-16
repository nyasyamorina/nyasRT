#pragma once

#include "../../common.hpp"
#include "../../geometry/Ray.hpp"


namespace nyasRT
{
namespace cameras
{
class Camera
{
public:

    virtual bool prepare() noexcept
    {
        return true;
    }

    virtual Ray cast_ray(vec2g const& screen_position /* in [-1,1] */) const noexcept = 0;
};

} // namespace cameras
} // namespace nyasRT