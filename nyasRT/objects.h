#pragma once

#include "setup.h"
#include <memory>
#include "utils.h"
#include "samples.h"


/* abstract types */

class BRDF;
class Texture;
class Object3D;
class Light;

typedef std::shared_ptr<BRDF> BRDFp;
typedef std::shared_ptr<Texture> Texturep;
typedef std::shared_ptr<Object3D> Object3Dp;
typedef std::shared_ptr<Light> Lightp;


/* pre-declaration */

class World;               // see `world.h`



/******************************************************************************
********************************  abstract BRDF  ******************************
******************************************************************************/

class BRDF {
public:
    virtual float f_r(Vec3 const& n, Vec3 const& i, Vec3 const& o) const = 0;
};


/******************************************************************************
******************************  abstract Texture  *****************************
******************************************************************************/

class Texture {
public:
    virtual RGB get_color(Vec2 const& uv) const = 0;
};


/******************************************************************************
******************************  abstract Object3D  ****************************
******************************************************************************/

struct Object3DArgs {
public:
    BRDFp brdf_p;
    Texturep texture_p;
    RGB emissive;

    Object3DArgs(BRDFp, Texturep);
    Object3DArgs(BRDFp, Texturep, RGB const&);
};

class Object3D {
public:
    BRDFp brdf_p;
    Texturep texture_p;
    RGB emissive;
    bool render_shadow = true;

    Object3D(Object3DArgs const&);

    bool is_light() const;
    RGB render(World const&, Ray const&, HitRecord const&);

    virtual double get_area() const = 0;
    virtual bool hit(Ray const&, double tmax) const = 0;
    virtual bool hit_record(Ray const&, HitRecord &) const = 0;
    virtual Vec3 get_sample(Vec3 & normal) = 0;
};


/******************************************************************************
********************************  abstract Light  *****************************
******************************************************************************/

class Light {
public:
    bool render_shadow = true;

    virtual RGB render(World const&, Ray const&, HitRecord const&) const = 0;
};



/******************************************************************************
*************************************  BRDFs  *********************************
******************************************************************************/

namespace BRDFs
{
    /*******************************  Lambertian  ****************************/

    class Lambertian final : public BRDF {
    public:
        virtual float f_r(Vec3 const&, Vec3 const&, Vec3 const&) const override;
    };
}


/******************************************************************************
***********************************  textures  ********************************
******************************************************************************/

namespace textures
{
    /*******************************  PureColor  *****************************/

    class PureColor final : public Texture {
    public:
        RGB color;

        PureColor(RGB const&);

        virtual RGB get_color(Vec2 const& uv) const;
    };
}


/******************************************************************************
**********************************  object3Ds  ********************************
******************************************************************************/

namespace object3Ds
{
    /*********************************  Sphere  ******************************/

    class Sphere final : public Object3D {
    public:
        Vec3 point;

        explicit Sphere(Object3DArgs const&, Vec3 const& p, double r);
        explicit Sphere(Object3DArgs const&, Vec3 const& p, double r, SamplerArgs const&);

        double get_radius() const;
        void set_radius();

        virtual double get_area() const override;
        virtual bool hit(Ray const&, double tmax) const override;
        virtual bool hit_record(Ray const&, HitRecord &) const override;
        virtual Vec3 get_sample(Vec3 & normal) override;

    private:
        double _r;
        double _area;
        Sampler<sample_types::Sphere> _sampler;
    };
}


/******************************************************************************
***********************************  lights  **********************************
******************************************************************************/

namespace lights
{

}
