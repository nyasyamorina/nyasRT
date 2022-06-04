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

    Light();

    virtual Vec3 get_direction(Vec3 const&) = 0;
    virtual RGB render_light(Vec3 const&, World const&) const = 0;
};


/******************************************************************************
**********************************  Light  ************************************
******************************************************************************/

class BRDF {
public:
    virtual float f_r(HitRecord const&, Vec3 const& i) const = 0;
    virtual Vec3 get_sample(HitRecord const&) = 0;
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
    virtual double get_area() const;
    virtual void get_sample(HitRecord &);
};



/******************************************************************************
*********************************  lights  ************************************
******************************************************************************/

namespace lights
{
    /******************************  Parallel  *******************************/

    class Parallel final : public Light {
    public:
        RGB color;
        Vec3 direcion;

        explicit Parallel(RGB const&, Vec3 const&);

        virtual Vec3 get_direction(Vec3 const&) override;
        virtual RGB render_light(Vec3 const&, World const&) const override;
    };


    /********************************  Point  ********************************/

    class Point : public Light {
    public:
        RGB color;
        Vec3 point;

        explicit Point(RGB const&, Vec3 const&);

        virtual Vec3 get_direction(Vec3 const&) override;
        virtual RGB render_light(Vec3 const&, World const&) const override;
    };


    /*****************************  ObjectLight  *****************************/

    class ObjectLight : public Light {
    public:
        Objectp object_p;
        HitRecord rec;

        explicit ObjectLight(Objectp, World & w);

        virtual Vec3 get_direction(Vec3 const&) override;
        virtual RGB render_light(Vec3 const&, World const&) const override;
    };


    /*************************  DualSideObjectLight  *************************/

    class DualSideObjectLight final : public ObjectLight {
    public:
        explicit DualSideObjectLight(Objectp, World & w);

        virtual RGB render_light(Vec3 const&, World const&) const override;
    };
}


/******************************************************************************
**********************************  brdfs  ************************************
******************************************************************************/

namespace brdfs
{
    /******************************  Lambertian  *****************************/

    class Lambertian final : public BRDF {
    public:
        Sampler<sample_types::Hemisphere> sampler;

        template<class... Args> explicit Lambertian(Args&&... args);

        virtual float f_r(HitRecord const&, Vec3 const&) const;
        virtual Vec3 get_sample(HitRecord const&);
    };

    template<class... Args> Lambertian::Lambertian(Args&&... args)
        : sampler(sample_types::Hemisphere(1.), std::forward<Args>(args)...) {}
}


/******************************************************************************
********************************  materials  **********************************
******************************************************************************/

namespace materials
{

    /*******************************  Emissive  ******************************/

    class Emissive : public Material {
    public:
        RGB color;

        Emissive(RGB const&);

        virtual RGB render_emissive(HitRecord const&);
    };


    /********************************  Matte  ********************************/
    class Matte final : public Material {
    public:
        typedef Samplerp<sample_types::Hemisphere> Samplerp;

        RGB color;
        Samplerp sampler_p;

        explicit Matte(RGB const&, Samplerp);

        virtual RGB render(HitRecord const&);
    };

    /********************************  Opaque  *******************************/

    class Opaque : public Material {
    public:
        BRDFp brdf_p;
        RGB color;

        explicit Opaque(BRDFp, RGB const&);

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
        Samplerp<sample_types::Sphere> sampler_p;

        explicit Sphere(Materialp, Vec3 const&, double);

        virtual bool hit(Ray const&, double);
        virtual bool hit_record(HitRecord &);
        virtual double get_area() const override;
        virtual void get_sample(HitRecord &) override;
    };


    /********************************  Circle  *******************************/

    class Circle final : public Object {
    public:
        Vec3 point, normal;
        double radius;
        Samplerp<sample_types::Circle> sampler_p;

        explicit Circle(Materialp, Vec3 const& p, Vec3 const& n, double r);

        virtual bool hit(Ray const&, double);
        virtual bool hit_record(HitRecord &);
        virtual double get_area() const override;
        virtual void get_sample(HitRecord &) override;
    };
}
