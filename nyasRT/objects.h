#pragma once

#include "setup.h"
#include "utils.h"
#include "samples.h"



/******************************************************************************
**********************************  Light  ************************************
******************************************************************************/

class Light {
public:
    bool has_shadow;

    explicit Light(bool shadow);

    virtual Vec3 get_direction(HitRecord const&) const = 0;
    virtual RGB render(HitRecord const&) const = 0;
};


/******************************************************************************
********************************  Material  ***********************************
******************************************************************************/

class Material {
public:
    virtual RGB render(HitRecord const&);
    virtual RGB render_emissive(HitRecord const&);
};


/******************************************************************************
*********************************  Object  ************************************
******************************************************************************/

class Object {
public:
    Materialp material_p;

    explicit Object(Materialp);

    bool is_light() const;

    virtual bool hit(Ray const&, double) = 0;
    virtual bool hit_record(HitRecord &) = 0;
    virtual bool has_sampler() const = 0;
    virtual Ray get_sample() = 0;
};



/******************************************************************************
*********************************  lights  ************************************
******************************************************************************/

namespace lights
{
    /*****************************  Directional  *****************************/

    class Directional final : public Light {
    public:
        RGB color;
        Vec3 direcion;

        explicit Directional(RGB const&, Vec3 const&, bool shadow = true);

        virtual Vec3 get_direction(HitRecord const&) const override;
        virtual RGB render(HitRecord const&) const override;
    };


    /******************************  PointLight  *****************************/

    class PointLight : public Light {
    public:
        RGB color;
        Vec3 point;

        explicit PointLight(RGB const&, Vec3 const&, bool shadow = true);

        virtual Vec3 get_direction(HitRecord const&) const override;
        virtual RGB render(HitRecord const&) const override;
    };


    /****************************  DecayPointLight  **************************/

    class DecayPointLight final : public PointLight {
    public:
        bool decay;
        double decay_coef;

        explicit DecayPointLight(RGB const&, Vec3 const&, double decaycoef = 2., bool shadow = true);

        virtual Vec3 get_direction(HitRecord const&) const override;
        virtual RGB render(HitRecord const&) const override;
    };
}


/******************************************************************************
********************************  materials  **********************************
******************************************************************************/

namespace materials
{

    /*******************************  Emissive  ******************************/

    class Emissive : public Material {};


    /********************************  Matte  ********************************/
    class Matte final : public Material {
    public:
        typedef Samplerp<sample_types::Hemisphere> Samplerp;

        RGB color;
        Samplerp sampler_p;

        explicit Matte(RGB const&, Samplerp);

        virtual RGB render(HitRecord const&);
    };

}


/******************************************************************************
*********************************  objects  ***********************************
******************************************************************************/

namespace objects
{
    /********************************  Sphere  *******************************/

    class Sphere final : public Object {
    public:
        Vec3 point;
        double radius;

        Sphere(Materialp, Vec3 const&, double);

        virtual bool hit(Ray const&, double);
        virtual bool hit_record(HitRecord &);
        virtual bool has_sampler() const;
        virtual Ray get_sample();
    };
}
