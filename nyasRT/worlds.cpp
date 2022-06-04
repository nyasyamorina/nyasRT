#include "worlds.h"

#include <iostream>
using namespace std;



/******************************************************************************
*********************************  Camera  ************************************
******************************************************************************/

Camera::Camera(uint64_t h, uint64_t w, FigureSamplerp s)
    : figure(h, w), sampler_p(s) {}


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
            return this->remap(rec.object_p->material_p->render_emissive(rec));
        }
        return rec.object_p->material_p->render(rec);
    }
    return RGB(0.);
}


/******************************************************************************
**********************************  World  ************************************
******************************************************************************/

World::World(uint32_t d)
    : ray_depth(d), tracer_p(nullptr), camera_p(nullptr) {}

HitRecord World::hit_object(Ray const& ray, double tmax) {
    auto rec = HitRecord(*this, ray, 0, tmax);
    for (auto & object_p : this->object_ps) {
        object_p->hit_record(rec);
    }
    if (rec.hit && dot(ray.direction, rec.normal) > 0.) {
        rec.normal = -rec.normal;
    }
    return rec;
}

bool World::v(Vec3 const& p, Vec3 const& q) const {
    auto d = q - p;
    auto dist = absnorm(d);
    return !this->hit_anything(Ray(p, d), dist);
}

bool World::hit_anything(Ray const& ray, double tmax) const {
    for (auto & object_p : this->object_ps) {
        if (object_p->hit(ray, tmax)) {
            return true;
        }
    }
    return false;
}



/******************************************************************************
*********************************  cameras  ***********************************
******************************************************************************/

using namespace cameras;

/********************************  Pinhole  **********************************/

Pinhole::Pinhole(uint64_t h, uint64_t w, FigureSamplerp s,
                 Vec3 const& p, Vec3 const& u_, Vec3 const& v_, Vec3 const& d)
    : Camera(h, w, s), point(p), u(u_), v(v_), direction(d) {}

Pinhole::Pinhole(uint64_t h, uint64_t w, FigureSamplerp s,
                 Vec3 const& p, Vec3 const& d, double fov, Vec3 const& viewup)
    : Camera(h, w, s), point(p), direction(d) {
    this->u = cross(d, viewup);
    this->v = cross(this->u, d);
    auto tmp = tan(fov / 2.) * abs(d);
    this->u *= tmp / abs(this->u);
    this->v *= tmp / abs(this->v) * h / w;
}

Ray Pinhole::get_ray(uint64_t h, uint64_t w) {
    auto & sample = this->sampler_p->next();
    auto tex_u = (w + sample.x) / this->figure.get_width() * 2. - 1.;
    auto tex_v = 1. - (h + sample.y) / this->figure.get_height() * 2.;
    auto d = tex_u * this->u + tex_v * this->v + this->direction;
    d *= 1. / abs(d);
    return Ray(this->point, d);
}


/******************************************************************************
*******************************  ray_tracers  *********************************
******************************************************************************/

using namespace ray_tracers;

/**********************************  RT1  ************************************/

RT1::RT1(World & w)
    : RayTracer(w) {}
RGB RT1::trace_from_camera(Ray const& ray, uint32_t depth) const {
    return this->trace(ray, depth);
}

RGB RT1::trace(Ray const& ray, uint32_t depth) const {
    if (depth == 0) {
        return RGB(0.f);
    }
    auto rec = this->world.hit_object(ray);
    rec.depth = depth;
    if (rec.hit) {
        return rec.object_p->material_p->render(rec);
    }
    auto const zenith = RGB(.7f, .9f, 1.f);
    auto const bottom = RGB(1.f);
    auto mix_v = (float)ray.direction.z * 0.5f + 0.5f;
    return zenith * mix_v + bottom * (1.f - mix_v);
}
