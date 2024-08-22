#pragma once

#include <math.h>
#include <memory>

#include "utils.hpp"
#include "geometry/vec3.hpp"
#include "geometry/vec4.hpp"
#include "geometry/Ray.hpp"
#include "geometry/Mesh.hpp"


// transform a `Mesh` from model space to world space
class Transform final
{
private:

    vec3g _rx, _ry, _rz;
    vec3g _ix, _iy, _iz;
    fg _scale;
    vec3g _offset;

public:

    CONST_FUNC Transform() noexcept
    : _rx{defaults<vec3g>::X}, _ry{defaults<vec3g>::Y}, _rz{defaults<vec3g>::Z}
    , _ix{defaults<vec3g>::X}, _iy{defaults<vec3g>::Y}, _iz{defaults<vec3g>::Z}
    , _scale{1}, _offset{defaults<vec3g>::O} {}

    CONST_FUNC Transform & rotate(vec3g const& axis, fg angle) noexcept
    {
        fg xx = axis.x*axis.x, xy = axis.x*axis.y, xz = axis.x*axis.z;
        fg yy = axis.y*axis.y, yz = axis.y*axis.z, zz = axis.z*axis.z;
        fg sa = std::sin(angle), ca = std::cos(angle);

        vec3g rx = vec3g(xx + (yy + zz) * ca, xy * (1 - ca) + axis.z * sa, xz * (1 - ca) - axis.y * sa);
        vec3g ry = vec3g(xy * (1 - ca) - axis.z * sa, yy + (xx + zz) * ca, yz * (1 - ca) + axis.x * sa);
        vec3g rz = vec3g(xz * (1 - ca) + axis.y * sa, yz * (1 - ca) - axis.x * sa, zz + (xx + yy) * ca);
        vec3g ox = _rx, oy = _ry, oz = _rz;
        _rx = ox.x * rx + ox.y * ry + ox.z * rz;
        _ry = oy.x * rx + oy.y * ry + oy.z * rz;
        _rz = oz.x * rx + oz.y * ry + oz.z * rz;
        _offset = _offset.x * rx + _offset.y * ry + _offset.z * rz;

        rx = vec3g(xx + (yy + zz) * ca, xy * (1 - ca) - axis.z * sa, xz * (1 - ca) + axis.y * sa);
        ry = vec3g(xy * (1 - ca) + axis.z * sa, yy + (xx + zz) * ca, yz * (1 - ca) - axis.x * sa);
        rz = vec3g(xz * (1 - ca) - axis.y * sa, yz * (1 - ca) + axis.x * sa, zz + (xx + yy) * ca);
        ox = _ix, oy = _iy, oz = _iz;
        _ix = rx.x * ox + rx.y * oy + rx.z * oz;
        _iy = ry.x * ox + ry.y * oy + ry.z * oz;
        _iz = rz.x * ox + rz.y * oy + rz.z * oz;

        return *this;
    }
    CONST_FUNC Transform & scale(fg scale_) noexcept
    {
        _scale *= scale_;
        _offset *= scale_;
        return *this;
    }
    CONST_FUNC Transform & offset(vec3g const& offset_) noexcept
    {
        _offset += offset_;
        return *this;
    }

    CONST_FUNC vec3g apply_normal(vec3g const& v) const noexcept
    {
        return v.x * _rx + v.y * _ry + v.z * _rz;
    }
    CONST_FUNC vec3g undo_normal(vec3g const& v) const noexcept
    {
        return v.x * _ix + v.y * _iy + v.z * _iz;
    }
    CONST_FUNC vec3g apply_vector(vec3g const& v) const noexcept
    {
        return apply_normal(v) *= _scale;
    }
    CONST_FUNC vec3g undo_vector(vec3g const& v) const noexcept
    {
        return undo_normal(v) /= _scale;
    }
    CONST_FUNC vec3g apply_point(vec3g const& p) const noexcept
    {
        return apply_vector(p) += _offset;
    }
    CONST_FUNC vec3g undo_point(vec3g const& p) const noexcept
    {
        return undo_vector(p - _offset);
    }

    CONST_FUNC Ray apply(Ray const& ray) const noexcept
    {
        return Ray(apply_point(ray.origin), apply_vector(ray.direction));
    }
    CONST_FUNC Ray undo(Ray const& ray) const noexcept
    {
        return Ray(undo_point(ray.origin), undo_vector(ray.direction));
    }
};


class Object3D
{
public:

    MeshPtr mesh_p;
    Transform transform;
    // TODO:

    Object3D() noexcept
    : mesh_p{nullptr}, transform{} {}
    Object3D(MeshPtr mesh) noexcept
    : mesh_p{mesh}, transform{} {}


    bool prepare()
    {
        if (mesh_p == nullptr) { return false; }
        return mesh_p->prepare();
    }


    bool trace(Ray const& ray, TraceRecord & rec) const noexcept
    {
        Ray model_ray = transform.undo(ray);
        if (mesh_p->trace(model_ray, rec))
        {
            rec.hit_point = transform.apply_point(rec.hit_point);
            rec.normal = transform.apply_normal(rec.normal);
            rec.object_p = this;
            return true;
        }
        return false;
    }
};

using Object3DPtr = std::shared_ptr<Object3D>;
using Object3DConstPtr = std::shared_ptr<Object3D const>;
