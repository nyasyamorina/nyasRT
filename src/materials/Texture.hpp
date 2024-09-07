#pragma once

#include <math.h>
#include <memory>

#include "../geometry/vec2.hpp"
#include "../Figure.hpp"
#include "../Interpolation.hpp"
#include "Material.hpp"


class Texture : public Material
{
public:

    Interpolation method;
    Figure * figure_p;
    bool owned_figure;  // if true, the figure inside will be deleted when this texture destroy.

    Texture() noexcept
    : method{Interpolation::Nearest}, figure_p{nullptr}, owned_figure{false} {}
    Texture(Interpolation method_, Figure * fig_p, bool owned_fig = false) noexcept
    : method{method_}, figure_p{fig_p}, owned_figure{owned_fig} {}
    virtual ~Texture() noexcept
    {
        if (owned_figure) { delete figure_p; }
    }

    Texture(Texture const& tex) noexcept
    : method{tex.method}, figure_p{new Figure(*tex.figure_p)}, owned_figure{true} {}
    Texture(Texture && tex) noexcept
    : method{tex.method}, figure_p{std::exchange(tex.figure_p, nullptr)}, owned_figure{std::exchange(tex.owned_figure, false)} {}

    Texture & remove_gamma() noexcept
    {
        if (figure_p != nullptr) for (RGB & pixel : *figure_p) { pixel.remove_gamma(); }
        return *this;
    }


    virtual bool prepare() noexcept override
    {
        return figure_p != nullptr;
    }

    virtual RGB operator () (Ray const& ray, TraceRecord const& rec) const noexcept override
    {
        return method(figure_p->begin(), figure_p->size(), rec.hit_texture);
    }


    // short cut load texture from file
    template<class Loader> static auto load(Interpolation method_, std::filesystem::path const& path, bool remove_gamma = true)
    {
        Figure * fig_p = new Figure;
        fig_p->load<Loader>(path);
        auto tex_p = std::make_shared<Texture>(method_, fig_p, true);
        if (remove_gamma) { tex_p->remove_gamma(); }
        return tex_p;
    }
};
