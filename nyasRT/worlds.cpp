#include "worlds.h"



/******************************************************************************
*********************************  Camera  ************************************
******************************************************************************/

Camera::Camera(uint64_t height, uint64_t width)
    : figure(height, width) {}


/******************************************************************************
*******************************  RayTracer  ***********************************
******************************************************************************/

RayTracer::RayTracer(World & w)
    : world(w) {}

RGB RayTracer::trace(Ray const& ray, uint32_t depth) const {
    if (depth == 0) {
        return RGB(0.f);
    }
    auto rec = this->world.hit_object(ray, inf);
    rec.depth = depth;
    if (rec.hit) {
        return rec.object_p->material_p->render(rec);
    }
    return RGB(0.);
}

RGB RayTracer::trace_from_camera(Ray const& ray, uint32_t depth) const {
    if (depth == 0) {
        return RGB(0.f);
    }
    auto rec = this->world.hit_object(ray, inf);
    rec.depth = depth;
    if (rec.hit) {
        if (rec.object_p->is_light()) {
            return rec.object_p->material_p->render_emissive(rec);
        }
        return rec.object_p->material_p->render(rec);
    }
    return RGB(0.);
}


/******************************************************************************
**********************************  World  ************************************
******************************************************************************/

World::World()
    : ambient_p(nullptr), camera_p(nullptr) {}
World::World(Ambientp amb, Camerap cam)
    : ambient_p(amb), camera_p(cam) {}

HitRecord World::hit_object(Ray const& ray, double tmax) {
    /* not the final version */
    auto rec = HitRecord(*this, ray, 0, tmax);
    for (auto & object_p : this->object_ps) {
        if (object_p->hit_record(rec)) {}
    }
    return rec;
}

bool World::v(Vec3 const& p, Vec3 const& q) const {
    auto d = q - p;
    auto dist = absnorm(d);
    return !this->hit_anything(Ray(p, d), dist);
}

bool World::hit_anything(Ray const& ray, double tmax) const {
    /* not the final version */
    for (auto & object_p : this->object_ps) {
        if (object_p->hit(ray, tmax)) {
            return true;
        }
    }
    return false;
}
