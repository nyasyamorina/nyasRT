#include "objects.h"

using namespace std;

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

bool Object::is_light() const {
    /* TODO */
    return false;
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
