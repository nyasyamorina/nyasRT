#pragma once

#include <filesystem>
#include <fstream>
#include <memory>

#include "../../graphics/GraphicsBuffer.hpp"
#include "../../graphics/Interpolation.hpp"
#include "Material.hpp"


namespace nyasRT
{
namespace materials
{
class Texture : public Material
{
public:

    Interpolation method;
    GraphicsBuffer * gbuf_p;
    bool owned_gbuf;  // if true, the gbuf inside will be deleted when this texture destroy.

    Texture() noexcept
    : method{Interpolation::Nearest}, gbuf_p{nullptr}, owned_gbuf{false} {}
    Texture(Interpolation method_, GraphicsBuffer * fig_p, bool owned_fig = false) noexcept
    : method{method_}, gbuf_p{fig_p}, owned_gbuf{owned_fig} {}
    virtual ~Texture() noexcept
    {
        if (owned_gbuf) { delete gbuf_p; }
    }

    Texture(Texture const& tex) noexcept
    : method{tex.method}, gbuf_p{new GraphicsBuffer(*tex.gbuf_p)}, owned_gbuf{true} {}
    Texture(Texture && tex) noexcept
    : method{tex.method}, gbuf_p{std::exchange(tex.gbuf_p, nullptr)}, owned_gbuf{std::exchange(tex.owned_gbuf, false)} {}

    Texture & remove_gamma() noexcept
    {
        if (gbuf_p != nullptr) for (RGB & pixel : *gbuf_p) { pixel = ::nyasRT::remove_gamma(pixel); }
        return *this;
    }


    virtual bool prepare() noexcept override
    {
        return gbuf_p != nullptr;
    }

    virtual RGB operator () (Ray const& ray, TraceRecord const& rec) const noexcept override
    {
        return method(gbuf_p->begin(), gbuf_p->size(), rec.hit_texture);
    }


    // short cut load texture from file
    template<class Loader> static auto load(Interpolation method, std::filesystem::path path, bool remove_gamma = true)
    {
        auto tex = std::make_shared<Texture>();
        tex->method = method;

        GraphicsBuffer * gbuf = new GraphicsBuffer;
        Loader loader;
        if (!(loader.load(path, *gbuf))) { delete gbuf; return tex; }
        tex->gbuf_p = gbuf;
        tex->owned_gbuf = true;

        if (remove_gamma) { tex->remove_gamma(); }
        return tex;
    }
};

} // namespace materials
} // namespace nyasRT
