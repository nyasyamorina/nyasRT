#include "objects.h"
#include "worlds.h"

#include "iostream"

using namespace std;
using namespace materials;
using namespace sample_types;

constexpr float OneOverPi = 1.0f / 3.141592653589793238462643383279502884f;



/******************************************************************************
**********************************  Light  ************************************
******************************************************************************/

Light::Light()
    : has_shadow(true) {}


/******************************************************************************
********************************  Material  ***********************************
******************************************************************************/

RGB Material::render(HitRecord const&) {
    return RGB(0.f);
}
RGB Material::render_emissive(HitRecord const&) {
    return RGB(0.f);
}


/******************************************************************************
*********************************  Object  ************************************
******************************************************************************/

Object::Object(Materialp mat)
    : material_p(mat) {}

bool Object::is_light() const {
    auto emissive_p = dynamic_cast<Emissive *>(this->material_p.get());
    return emissive_p != nullptr;
}

bool Object::has_sampler() const {
    return false;
}
Ray Object::get_sample() {
    return Ray();
}


/******************************************************************************
*********************************  lights  ************************************
******************************************************************************/

using namespace lights;

/********************************  Parallel  *********************************/

Parallel::Parallel(RGB const& c, Vec3 const& d)
    : color(c), direcion(normalize(d)) {}

Vec3 Parallel::get_direction(Vec3 const&) {
    return -this->direcion;
}
RGB Parallel::render_light(Vec3 const& p, World const& world) const {
    if (this->has_shadow && world.hit_anything(Ray(p, -this->direcion), inf)) {
        return RGB(0.f);
    }
    return this->color;
}


/**********************************  Point  **********************************/

Point::Point(RGB const& c, Vec3 const& q)
    : color(c), point(q) {}

Vec3 Point::get_direction(Vec3 const& p) {
    return normalize(this->point - p);
}
RGB Point::render_light(Vec3 const& p, World const& world) const {
    auto dire = this->point - p;
    auto d = absnorm(dire);
    if (this->has_shadow &&
        world.hit_anything(Ray(p, dire), d)) {
        return RGB(0.f);
    }
    return this->color * (float)(1. / (d * d));
}


/******************************************************************************
**********************************  brdfs  ************************************
******************************************************************************/

using namespace brdfs;

/********************************  Lambertian  *******************************/

float Lambertian::f_r(HitRecord const&, Vec3 const&) const {
    return OneOverPi;
}
Vec3 Lambertian::get_sample(HitRecord const& rec) {
    return LocalCoord(rec.normal).at(this->sampler.next());
}


/******************************************************************************
********************************  materials  **********************************
******************************************************************************/

using namespace materials;

/**********************************  Matte  **********************************/

Matte::Matte(RGB const& c, Samplerp s)
    : color(c), sampler_p(s) {}

RGB Matte::render(HitRecord const& rec) {
    return this->color * rec.world.tracer_p->trace(Ray(rec.point, LocalCoord(rec.normal).at(this->sampler_p->next())), rec.depth - 1);
}


/********************************  Opaque  *******************************/

Opaque::Opaque(BRDFp b, RGB const& c)
    : brdf_p(b), color(c) {}

RGB Opaque::render(HitRecord const& rec) {
    // Hemisphere rendering model
    auto L = rec.world.tracer_p->trace(
        Ray(rec.point, this->brdf_p->get_sample(rec)),
        rec.depth - 1
    );
    for (auto & light_p : rec.world.light_ps) {
        // Area rendering model
        auto omega_i = light_p->get_direction(rec.point);
        auto cos_theta = (float)dot(rec.normal, omega_i);
        if (cos_theta > 0.f) {
            auto tmp = this->brdf_p->f_r(rec, omega_i) * cos_theta;
            L += tmp * light_p->render_light(rec.point, rec.world);
        }
        // else: `cos_theta < 0` means the light is behind the object,
        // and there is no need to render the light.
    }
    return this->color * L;
}


/******************************************************************************
*********************************  objects  ***********************************
******************************************************************************/

using namespace objects;

/**********************************  Sphere  *********************************/

objects::Sphere::Sphere(Materialp mat,  Vec3 const& p, double r)
    : Object(mat), point(p), radius(r) {}

bool objects::Sphere::hit(Ray const& ray, double tmax) {
    auto p = ray.point - this->point;
    auto half_b = dot(ray.direction, p);
    auto d = half_b * half_b - abs2(p) + this->radius * this->radius;
    if (d < 0.) {
        return false;
    }
    d = sqrt(d);
    auto t = -d - half_b;
    if (t < 1e-8) {
        t = d - half_b;
        if (t < 1e-8) {
            return false;
        }
    }
    return t < tmax;
}
bool objects::Sphere::hit_record(HitRecord & rec) {
    auto p = rec.ray.point - this->point;
    auto half_b = dot(rec.ray.direction, p);
    auto d = half_b * half_b - abs2(p) + this->radius * this->radius;
    if (d < 0.) {
        return false;
    }
    d = sqrt(d);
    auto t = -d - half_b;
    if (t < 1e-8) {
        t = d - half_b;
        if (t < 1e-8) {
            return false;
        }
    }
    /* something similar in `hit` */
    if (t >= rec.t) {
        return false;
    }
    rec.hit = true;
    rec.object_p = this;
    rec.t = t;
    rec.point = rec.ray.at(t);
    rec.normal = (rec.point - this->point) / this->radius;
    rec.tex/* not useful for now */;
    return true;
}
