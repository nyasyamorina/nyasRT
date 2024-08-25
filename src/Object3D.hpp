#pragma once

#include <memory>

#include "utils.hpp"
#include "geometry/vec3.hpp"
#include "geometry/vec4.hpp"
#include "geometry/Ray.hpp"
#include "geometry/Mesh.hpp"
#include "geometry/Transform.hpp"
#include "BRDFs/BRDF.hpp"


class Object3D
{
public:

    MeshPtr mesh_p;
    Transform transform;
    BRDFPtr brdf_p;

    Object3D() noexcept
    : mesh_p{nullptr}, transform{}, brdf_p{nullptr} {}
    Object3D(MeshPtr mesh, BRDFPtr brdf) noexcept
    : mesh_p{mesh}, transform{}, brdf_p(brdf) {}


    bool prepare()
    {
        if ((mesh_p == nullptr) || (brdf_p == nullptr)) { return false; }
        return mesh_p->prepare() && brdf_p->prepare();
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
