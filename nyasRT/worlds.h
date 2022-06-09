#pragma once

#include <vector>
#include "setup.h"
#include "utils.h"
#include "samples.h"
#include "objects.h"



/******************************************************************************
*********************************  Ambient  ***********************************
******************************************************************************/

class Ambient {
public:
    RGB color;

    explicit Ambient(RGB const&);

    virtual RGB render(HitRecord const&) const;
};


/******************************************************************************
*********************************  Camera  ************************************
******************************************************************************/

class Camera {
public:
    typedef Samplerp<sample_types::Square> FigureSamplerp;

    Image figure;
    FigureSamplerp sampler_p;

    explicit Camera(uint64_t height, uint64_t width, FigureSamplerp);

    virtual Ray get_ray(uint64_t h, uint64_t w) = 0;
};


/******************************************************************************
*******************************  RayTracer  ***********************************
******************************************************************************/

class RayTracer {
public:
    World & world;
    remap_colors::MaxTo01 remap;

    explicit RayTracer(World &);

    virtual RGB trace(Ray const&, uint32_t depth) const;
    virtual RGB trace_from_camera(Ray const&, uint32_t depth) const;
};


/******************************************************************************
**********************************  World  ************************************
******************************************************************************/

class World final {
public:
    uint32_t ray_depth;
    RayTracerp tracer_p;
    Camerap camera_p;
    std::vector<Objectp> object_ps;
    std::vector<Lightp> light_ps;

    explicit World(uint32_t);

    HitRecord hit_object(Ray const&, double tmax = inf);
    bool hit_anything(Ray const&, double tmax = inf) const;
    bool v(Vec3 const&,  Vec3 const&) const;
};



/******************************************************************************
*********************************  cameras  ***********************************
******************************************************************************/

namespace cameras
{
    /******************************  Pinhole  ********************************/

    class Pinhole : public Camera {
    public:
        Vec3 point, direction, u, v;

        explicit Pinhole(uint64_t, uint64_t, FigureSamplerp, Vec3 const& p,
                         Vec3 const& u_, Vec3 const& v_, Vec3 const& d);
        explicit Pinhole(uint64_t, uint64_t, FigureSamplerp, Vec3 const& p,
                         Vec3 const& d, double fov, Vec3 const& viewup = Vec3(0, 0, 1));

        virtual Ray get_ray(uint64_t h, uint64_t w);
    };


    /******************************  Parallel  *******************************/

    class Parallel final : public Camera {
    public:
        Vec3 point, direction, u, v;

        explicit Parallel(uint64_t, uint64_t, FigureSamplerp, Vec3 const& p,
                          Vec3 const& u_, Vec3 const& v_, Vec3 const& d);
        explicit Parallel(uint64_t, uint64_t, FigureSamplerp, Vec3 const& p,
                          Vec3 const& d, double wide, Vec3 const& viewup = Vec3(0, 0, 1));

        virtual Ray get_ray(uint64_t h, uint64_t w);
    };


    /********************************  Lens  *********************************/

    class Lens final : public Pinhole {
    public:
        typedef Samplerp<sample_types::Circle> LensSamplerp;

        double lens_radius;
        LensSamplerp len_sampler_p;

        explicit Lens(uint64_t, uint64_t, FigureSamplerp, Vec3 const& p,
                      Vec3 const& u_, Vec3 const& v_, Vec3 const& d,
                      double r, LensSamplerp);
        explicit Lens(uint64_t, uint64_t, FigureSamplerp, Vec3 const& p,
                      Vec3 const& d, double fov, double r, LensSamplerp,
                      Vec3 const& viewup = Vec3(0, 0, 1));

        virtual Ray get_ray(uint64_t h, uint64_t w);
    };


    /*******************************  Fisheye  *******************************/

    class Fisheye final : public Camera {
    public:
        Vec3 point, direction, u, v;
        double fov;

        explicit Fisheye(uint64_t, uint64_t, FigureSamplerp, Vec3 const& p,
                         Vec3 const& d, double fov_, Vec3 const& viewup = Vec3(0, 0, 1));

        virtual Ray get_ray(uint64_t h, uint64_t w);
    };


    /******************************  Spherical  ******************************/

    class Spherical : public Camera {
    public:
        Vec3 point, direction, u, v;

        explicit Spherical(uint64_t, uint64_t, FigureSamplerp, Vec3 const& p,
                           Vec3 const& d, Vec3 const& viewup = Vec3(0, 0, 1));

        virtual Ray get_ray(uint64_t h, uint64_t w);
    };
}


/******************************************************************************
*******************************  ray_tracers  *********************************
******************************************************************************/

namespace ray_tracers
{
    /********************************  RT1  **********************************/

    class RT1 final : public RayTracer {
    public:
        explicit RT1(World &);

        virtual RGB trace(Ray const&, uint32_t depth) const;
        virtual RGB trace_from_camera(Ray const&, uint32_t depth) const;
    };
}
