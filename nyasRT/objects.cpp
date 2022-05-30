#include "objects.h"
#include "worlds.h"

#include "iostream"

using namespace std;
using namespace materials;

constexpr float delta_color = 1.f / 255.f;
constexpr float OneOverPi = 1.0f / 3.141592653589793238462643383279502884f;



/******************************************************************************
**********************************  Light  ************************************
******************************************************************************/

Light::Light(bool s)
    : has_shadow(s) {}


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


/******************************************************************************
*********************************  lights  ************************************
******************************************************************************/

using namespace lights;

/*******************************  Directional  *******************************/

Directional::Directional(RGB const& c, Vec3 const& d, bool s)
    : Light(s), color(c), direcion(normalize(d)) {}

Vec3 Directional::get_direction(HitRecord const&) const {
    return -this->direcion;
}
RGB Directional::render(HitRecord const&) const {
    return this->color;
}


/******************************  PointLight  *****************************/

PointLight::PointLight(RGB const& c, Vec3 const& p, bool s)
    : Light(s), color(c), point(p) {}

Vec3 PointLight::get_direction(HitRecord const& rec) const {
    return Vec3(0);
}
RGB PointLight::render(HitRecord const& rec) const {
    return this->color;
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


/******************************************************************************
*********************************  objects  ***********************************
******************************************************************************/

using namespace objects;

/**********************************  Sphere  *********************************/

objects::Sphere::Sphere(Materialp mat,  Vec3 const& p, double r)
    : Object(mat), point(p), radius(r) {}

bool objects::Sphere::has_sampler() const {
    return false;
}
Ray objects::Sphere::get_sample() {
    return Ray();
}

bool objects::Sphere::hit(Ray const& ray, double tmax) {
    auto p = ray.point - this->point;
    auto a = abs2(ray.direction);
    auto half_b = dot(ray.direction, p);
    auto c = abs2(p) - this->radius * this->radius;
    auto d = half_b * half_b - a * c;
    if (d < 0.) {
        return false;
    }
    d = sqrt(d);
    auto a_t = -d - half_b;
    if (a_t < 1e-8) {
        a_t = d - half_b;
        if (a_t < 1e-8) {
            return false;
        }
    }
    return a_t < a * tmax;
}

bool objects::Sphere::hit_record(HitRecord & rec) {
    auto p = rec.ray.point - this->point;
    auto a = abs2(rec.ray.direction);
    auto half_b = dot(rec.ray.direction, p);
    auto c = abs2(p) - this->radius * this->radius;
    auto d = half_b * half_b - a * c;
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
    t /= a;
    /* something similar in `hit` */
    if (t >= rec.t) {
        return false;
    }
    rec.hit = true;
    rec.object_p = this;
    rec.t = t;
    rec.point = rec.ray.at(t);
    rec.normal = rec.point;
    rec.normal -= this->point;
    rec.normal /= this->radius;
    rec.tex/* not useful for now */;
    return true;
}

//bool Sphere::hit_record(HitRecord & rec) {
//    /* something similar in `hit` */
//    double t/* = a_t / a */;
//    if (t > rec.t) {
//        return false;
//    }
//    rec.hit = true;
//    rec.object_p = this;
//    rec.t = t;
//    rec.point = rec.ray.at(t);
//    rec.normal = rec.point;
//    rec.normal -= this->point;
//    rec.normal /= this->radius;
//    rec.tex/* not useful for now */;
//    return true;
//}
