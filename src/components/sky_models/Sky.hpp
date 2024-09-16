#pragma once

#include "../../common.hpp"


namespace nyasRT
{
namespace sky_models
{
class Sky
{
public:

    virtual bool prepare() noexcept
    {
        return true;
    }

    virtual RGB operator () (normal3g const& direction) const noexcept = 0;
};

} // namespace sky_models
} // namespace nyasRT
