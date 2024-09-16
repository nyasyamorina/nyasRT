#pragma once

#include <memory>

#include "../common.hpp"
#include "../geometry/Ray.hpp"
#include "../geometry/Mesh.hpp"
#include "../geometry/Transform.hpp"
#include "BRDFs/BRDF.hpp"
#include "materials/Material.hpp"


namespace nyasRT
{
class Object3D
{
public:

    using MeshPtr = std::shared_ptr<Mesh>;
    using MaterialPtr = std::shared_ptr<materials::Material>;
    using BRDFPtr = std::shared_ptr<BRDFs::BRDF>;

    MeshPtr mesh_p;
    Transform transform;
    MaterialPtr material_p;
    BRDFPtr brdf_p;

    Object3D() noexcept
    : mesh_p{nullptr}, transform{}, material_p{nullptr}, brdf_p{nullptr} {}
    Object3D(MeshPtr mesh, MaterialPtr material, BRDFPtr brdf) noexcept
    : mesh_p{mesh}, transform{}, material_p{material}, brdf_p{brdf} {}


    bool prepare()
    {
        if ((mesh_p == nullptr) || (material_p == nullptr) || (brdf_p == nullptr)) { return false; }
        return mesh_p->prepare() && material_p->prepare() && brdf_p->prepare();
    }


    bool trace(Ray const& ray, TraceRecord & rec) const noexcept
    {
        Ray model_ray = transform.undo(ray);
        if (mesh_p->trace(model_ray, rec))
        {
            rec.hit_point = transform.apply_point(rec.hit_point);
            rec.face_normal = transform.apply_normal(rec.face_normal);
            rec.hit_normal = transform.apply_normal(rec.hit_normal);
            rec.object_p = this;
            return true;
        }
        return false;
    }

    bool test_hit(Ray const& ray, fg max_ray_time) const noexcept
    {
        Ray model_ray = transform.undo(ray);
        return mesh_p->test_hit(model_ray, max_ray_time);
    }
};

} // namespace nyasRT
