#pragma once

#include <memory>
#include <thread>
#include <vector>

#include "common.hpp"
#include "components/Object3D.hpp"
#include "geometry/Ray.hpp"
#include "components/cameras/Camera.hpp"
#include "components/sky_models/Sky.hpp"
#include "components/light_sources/LightSource.hpp"


namespace nyasRT
{
class Scence
{
public:

    using CameraPtr = std::shared_ptr<cameras::Camera>;
    using LightSourcePtr = std::shared_ptr<light_sources::LightSource>;
    using SkyPtr = std::shared_ptr<sky_models::Sky>;

protected:

    bool _prepared;
    CameraPtr _camera_p;
    SkyPtr _sky_p;

    bool _prepare()
    {
        if (!has_camera() || (!_camera_p->prepare())) { return false; }
        if (has_sky() && (!_sky_p->prepare())) { return false; }

        for (LightSourcePtr & light_p : light_ps)
        {
            if (!light_p->prepare()) { return false; }
        }
        for (Object3D & object : objects)
        {
            if (!object.prepare()) { return false; }
        }
        return true;
    }

public:

    std::vector<LightSourcePtr> light_ps;
    std::vector<Object3D> objects;

    Scence() noexcept
    : _prepared(false), _camera_p{nullptr}, _sky_p{nullptr} {}


    bool prepare()
    {
        return _prepared = _prepare();
    }
    bool prepared() const noexcept
    {
        return _prepared;
    }

    bool has_camera() const noexcept
    {
        return _camera_p != nullptr;
    }
    bool has_sky() const noexcept
    {
        return _sky_p != nullptr;
    }


    Scence & camera(CameraPtr camera_) noexcept
    {
        _camera_p = camera_;
        return *this;
    }
    Scence & sky(SkyPtr sky_) noexcept
    {
        _sky_p = sky_;
        return *this;
    }

    CameraPtr camera() noexcept
    {
        return _camera_p;
    }
    CameraPtr camera() const noexcept
    {
        return _camera_p;
    }
    cameras::Camera const& camera_ref() const noexcept
    {
        return *_camera_p;
    }
    SkyPtr sky() noexcept
    {
        return _sky_p;
    }
    SkyPtr sky() const noexcept
    {
        return _sky_p;
    }
    sky_models::Sky const& sky_ref() const noexcept
    {
        return *_sky_p;
    }


    bool trace(Ray const& ray, TraceRecord & rec) const noexcept
    {
        bool hit = false;
        for (Object3D const& object : objects)
        {
            hit |= object.trace(ray, rec);
        }
        return hit;
    }

    bool test_hit(Ray const& ray, fg max_ray_time) const noexcept
    {
        for (Object3D const& object : objects)
        {
            if (object.test_hit(ray, max_ray_time)) { return true; }
        }
        return false;
    }
};

} // namespace nyasRT
