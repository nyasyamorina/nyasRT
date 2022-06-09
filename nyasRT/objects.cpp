#include "objects.h"
#include "worlds.h"

using namespace std;
using namespace materials;
using namespace sample_types;

constexpr double Pi = 3.141592653589793238462643383279502884;
constexpr float OneOverPi = 1.0f / (float)Pi;
constexpr double FourPi = 4.0 * Pi;



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

double Object::get_area() const {
    return nanf64;
}
void Object::get_sample(HitRecord &) {}


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


/*******************************  ObjectLight  *******************************/

ObjectLight::ObjectLight(Objectp o, World & w)
    : object_p(o), rec(w, Ray(), 0, inf) {}

Vec3 ObjectLight::get_direction(Vec3 const& p) {
    this->rec.ray.point = p;
    this->object_p->get_sample(this->rec);
    return this->rec.ray.direction;
}
RGB ObjectLight::render_light(Vec3 const& p, World const& w) const {
    auto cos_theta_o = -dot(this->rec.normal, this->rec.ray.direction);
    if (cos_theta_o < 0.) {     // means the ray is coming from behind the object
        return RGB(0.f);
    }
    if (this->has_shadow &&
        w.hit_anything(this->rec.ray, this->rec.t * eps_1)) {
        return RGB(0.f);
    }
    auto tmp = this->object_p->get_area() * cos_theta_o / (this->rec.t * this->rec.t);
    return this->object_p->material_p->render_emissive(this->rec) * (float)tmp;
}


/***************************  DualSideObjectLight  ***************************/

DualSideObjectLight::DualSideObjectLight(Objectp o, World & w)
    : ObjectLight(o, w) {}

RGB DualSideObjectLight::render_light(Vec3 const& p, World const& w) const {
    if (this->has_shadow &&
        w.hit_anything(this->rec.ray, this->rec.t * eps_1)) {
        return RGB(0.f);
    }
    auto cos_theta_o = abs(dot(this->rec.normal, this->rec.ray.direction));
    auto tmp = this->object_p->get_area() * cos_theta_o / (this->rec.t * this->rec.t);
    return this->object_p->material_p->render_emissive(this->rec) * (float)tmp;
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

/*********************************  Emissive  ********************************/

Emissive::Emissive(RGB const& c)
    : color(c) {}

RGB Emissive::render_emissive(HitRecord const&) {
    return this->color;
}


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
        auto cos_theta_i = (float)dot(rec.normal, omega_i);
        if (cos_theta_i > 0.f) {
            auto tmp = this->brdf_p->f_r(rec, omega_i) * cos_theta_i;
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

objects::Sphere::Sphere(Materialp mat, Vec3 const& p, double r)
    : Object(mat), point(p), radius(r), sampler_p(nullptr) {}

bool objects::Sphere::hit(Ray const& ray, double tmax) {
    auto p = ray.point - this->point;
    auto half_b = dot(ray.direction, p);
    auto d = half_b * half_b - abs2(p) + this->radius * this->radius;
    if (d < 0.) {
        return false;
    }
    d = sqrt(d);
    auto t = -d - half_b;
    if (t < eps) {
        t = d - half_b;
        if (t < eps) {
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
    if (t < eps) {
        t = d - half_b;
        if (t < eps) {
            return false;
        }
    }
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

double objects::Sphere::get_area() const {
    return FourPi * this->radius * this->radius;
}
void objects::Sphere::get_sample(HitRecord & rec) {
    rec.hit = true;
    rec.object_p = this;
    rec.normal = this->sampler_p->next();
    rec.point = rec.normal * this->radius + this->point;
    rec.ray.direction = rec.point - rec.ray.point;
    rec.t = absnorm(rec.ray.direction);
    rec.tex/* = not useful for now */;
}


/********************************  Circle  *******************************/


objects::Circle::Circle(Materialp mat, Vec3 const& p, Vec3 const& n, double r)
    : Object(mat), point(p), normal(normalize(n)), radius(r), sampler_p(nullptr) {}

bool objects::Circle::hit(Ray const& ray, double tmax) {
    auto t = dot(this->point - ray.point, this->normal) /
             dot(ray.direction, this->normal);
    if (t < eps || t > tmax) {
        return false;
    }
    return abs2(ray.at(t) - this->point) < this->radius * this->radius;
}
bool objects::Circle::hit_record(HitRecord & rec) {
    auto t = dot(this->point - rec.ray.point, this->normal) /
             dot(rec.ray.direction, this->normal);
    if (t < eps || t > rec.t) {
        return false;
    }
    auto p = rec.ray.at(t);
    if (abs2(p - this->point) > this->radius * this->radius) {
        return false;
    }
    rec.hit = true;
    rec.object_p = this;
    rec.t = t;
    rec.point = p;
    rec.normal = this->normal;
    rec.tex/* = not useful for now */;
    return true;
}

double objects::Circle::get_area() const {
    return Pi * this->radius * this->radius;
}
void objects::Circle::get_sample(HitRecord & rec) {
    auto & sample = this->sampler_p->next();
    rec.hit = true;
    rec.object_p = this;
    rec.normal = this->normal;
    rec.point = LocalCoord(rec.normal).at(Vec3(sample.x, sample.y, 0.)) *
                this->radius + this->point;
    rec.ray.direction = rec.point - rec.ray.point;
    rec.t = absnorm(rec.ray.direction);
    rec.tex/* = not useful for now */;
}
