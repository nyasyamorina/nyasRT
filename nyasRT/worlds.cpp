#include "worlds.h"

constexpr double Pi = 3.141592653589793238462643383279502884;
constexpr double PiOverTwo = Pi / 2.;



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


/********************************  Parallel  *********************************/

cameras::Parallel::Parallel(uint64_t h, uint64_t w, FigureSamplerp s, Vec3 const& p,
                   Vec3 const& u_, Vec3 const& v_, Vec3 const& d)
    : Camera(h, w, s), point(p), direction(normalize(d)), u(u_), v(v_) {}

cameras::Parallel::Parallel(uint64_t h, uint64_t w, FigureSamplerp s,
                   Vec3 const& p, Vec3 const& d, double wide, Vec3 const& viewup)
    : Camera(h, w, s), point(p), direction(normalize(d)) {
    this->u = cross(d, viewup);
    this->v = cross(this->u, d);
    this->u *= wide / 2. / abs(this->u);
    this->v *= wide / 2. / abs(this->v) * h / w;
}

Ray cameras::Parallel::get_ray(uint64_t h, uint64_t w) {
    auto & sample = this->sampler_p->next();
    auto tex_u = (w + sample.x) / this->figure.get_width() * 2. - 1.;
    auto tex_v = 1. - (h + sample.y) / this->figure.get_height() * 2.;
    auto p = tex_u * this->u + tex_v * this->v + this->point;
    return Ray(p, this->direction);
}


/**********************************  Lens  ***********************************/

Lens::Lens(uint64_t h, uint64_t w, FigureSamplerp fs, Vec3 const& p,
           Vec3 const& u_, Vec3 const& v_, Vec3 const& d, double r, LensSamplerp ls)
    : Pinhole(h, w, fs, p, u_, v_, d), lens_radius(r), len_sampler_p(ls) {}

Lens::Lens(uint64_t h, uint64_t w, FigureSamplerp fs, Vec3 const& p,
           Vec3 const& d, double fov, double r, LensSamplerp ls, Vec3 const& viewup)
    : Pinhole(h, w, fs, p, d, fov, viewup), lens_radius(r), len_sampler_p(ls) {}


Ray Lens::get_ray(uint64_t h, uint64_t w) {
    auto & fsamp = this->sampler_p->next();
    auto tex_u = (w + fsamp.x) / this->figure.get_width() * 2. - 1.;
    auto tex_v = 1. - (h + fsamp.y) / this->figure.get_height() * 2.;
    auto d = tex_u * this->u + tex_v * this->v + this->direction;
    auto & lsamp = this->len_sampler_p->next();
    auto p = lsamp.x * normalize(this->u) + lsamp.y * normalize(this->v);
    p *= this->lens_radius;
    return Ray(this->point + p, normalize(d - p));
}


/*********************************  Fisheye  *********************************/

Fisheye::Fisheye(uint64_t h, uint64_t w, FigureSamplerp s, Vec3 const& p,
                     Vec3 const& d, double fov_, Vec3 const& viewup)
    : Camera(h, w, s), point(p), direction(normalize(d)), fov(fov_) {
    this->u = normalize(cross(d, viewup));
    this->v = cross(this->u, this->direction);
}

Ray Fisheye::get_ray(uint64_t h, uint64_t w) {
    auto & sample = this->sampler_p->next();
    auto half_width = this->figure.get_width() / 2.;
    auto w_shift = w + sample.x - half_width;
    auto h_shift = this->figure.get_height() / 2. - h - sample.y;
    auto r = sqrt(h_shift * h_shift + w_shift * w_shift) / half_width;
    auto tex = spherical(1., r * this->fov / 2., atan2(h_shift, w_shift));
    auto d = tex.x * this->u + tex.y * this->v + tex.z * this->direction;
    return Ray(this->point, d);
}


/********************************  Spherical  ********************************/

Spherical::Spherical(uint64_t h, uint64_t w, FigureSamplerp s,
                     Vec3 const& p, Vec3 const& d, Vec3 const& viewup)
    : Camera(h, w, s), point(p), direction(normalize(d)) {
    this->u = normalize(cross(d, viewup));
    this->v = cross(this->u, this->direction);
}

Ray Spherical::get_ray(uint64_t h, uint64_t w) {
    auto & sample = this->sampler_p->next();
    auto tex_u = (w + sample.x) / this->figure.get_width() * 2. - 1.;
    auto tex_v = 1. - (h + sample.y) / this->figure.get_height() * 2.;
    auto tex = spherical(1., PiOverTwo * (1. - tex_v), Pi * tex_u);
    auto d = tex.x * this->direction + tex.y * this->u + tex.z * this->v;
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
