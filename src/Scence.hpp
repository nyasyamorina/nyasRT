#pragma once

#include <thread>
#include <vector>

#include "utils.hpp"
#include "Object3D.hpp"
#include "cameras/Camera.hpp"


class Renderer;

class Scence
{
protected:

    bool _prepared;
    CameraPtr _camera_p;
    // TODO:

    bool _prepare()
    {
        if (_camera_p == nullptr) { return false; }
        if (!_camera_p->prepare()) { return false; }
        for (Object3D & object : objects)
        {
            if (!object.prepare()) { return false; }
        }
        return true;
    }

public:

    std::vector<Object3D> objects;

    Scence() noexcept
    : _prepared(false), _camera_p{nullptr}, objects{} {}


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

    Scence & set_camera(CameraPtr camera_) noexcept
    {
        _camera_p = camera_;
        return *this;
    }
};
