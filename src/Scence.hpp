#pragma once

#include <thread>
#include <vector>

#include "utils.hpp"
#include "Object3D.hpp"
#include "cameras/Camera.hpp"
#include "sky_models/Sky.hpp"


class Scence
{
protected:

    bool _prepared;
    CameraPtr _camera_p;
    SkyPtr _sky_p;

    bool _prepare()
    {
        if (!has_camera() || (!_camera_p->prepare())) { return false; }
        if (has_sky() && (!_sky_p->prepare())) { return false; }

        for (Object3D & object : objects)
        {
            if (!object.prepare()) { return false; }
        }
        return true;
    }

public:

    std::vector<Object3D> objects;

    Scence() noexcept
    : _prepared(false), _camera_p{nullptr}, _sky_p{nullptr}, objects{} {}


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
    CameraConstPtr camera() const noexcept
    {
        return _camera_p;
    }
    Camera const& camera_ref() const noexcept
    {
        return *_camera_p;
    }
    SkyPtr sky() noexcept
    {
        return _sky_p;
    }
    SkyConstPtr sky() const noexcept
    {
        return _sky_p;
    }
    Sky const& sky_ref() const noexcept
    {
        return *_sky_p;
    }
};
