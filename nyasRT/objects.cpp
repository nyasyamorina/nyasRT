#include "objects.h"

using namespace std;

constexpr float delta_color = 1.f / 255.f;
constexpr float OneOverPi = 1.0f / 3.141592653589793238462643383279502884f;


/******************************************************************************
******************************  abstract Object3D  ****************************
******************************************************************************/

Object3DArgs::Object3DArgs(BRDFp br_p, Texturep tex_p)
    : brdf_p(br_p), texture_p(tex_p), emissive(0.0f) {}
Object3DArgs::Object3DArgs(BRDFp br_p, Texturep tex_p, RGB const& em)
    : brdf_p(br_p), texture_p(tex_p), emissive(em) {}

Object3D::Object3D(Object3DArgs const& args)
    : brdf_p(args.brdf_p), texture_p(args.texture_p), emissive(args.emissive) {}

bool Object3D::is_light() const {
    return this->emissive.r > delta_color || this->emissive.g > delta_color || this->emissive.b > delta_color;
}

//RGB Object3D::render(World const& world, Ray const& ray, HitRecord const& rec) {}


/******************************************************************************
*************************************  BRDFs  *********************************
******************************************************************************/

using namespace BRDFs;

/*********************************  Lambertian  ******************************/

float Lambertian::f_r(Vec3 const&, Vec3 const&, Vec3 const&) const {
    return OneOverPi;
}
