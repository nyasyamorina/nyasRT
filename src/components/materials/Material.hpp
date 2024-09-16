#pragma once

#include "../../common.hpp"
#include "../../geometry/Ray.hpp"


namespace nyasRT
{
namespace materials
{
class Material
{
public:

    virtual bool prepare() noexcept
    {
        return true;
    }

    virtual RGB operator () (Ray const& ray, TraceRecord const& rec) const noexcept = 0;
};

} // namespace materials
} // namespace nyasRT
