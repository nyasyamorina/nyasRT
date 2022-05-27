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
    Image figure;

    explicit Camera(uint64_t height, uint64_t width);

    virtual Ray get_ray(uint64_t h, uint64_t w) = 0;
    virtual Vec2 to_figure(Vec3 const& p) const = 0;
};


/******************************************************************************
*******************************  RayTracer  ***********************************
******************************************************************************/

class RayTracer {
public:
    World & world;

    RayTracer(World &);

    virtual RGB trace(Ray const&, uint32_t depth) const;
    virtual RGB trace_from_camera(Ray const&, uint32_t depth) const;
};


/******************************************************************************
**********************************  World  ************************************
******************************************************************************/

class World final {
public:
    RayTracerp tracer_p;
    Ambientp ambient_p;
    Camerap camera_p;
    std::vector<Objectp> object_ps;
    std::vector<Lightp> light_ps;

    World();
    explicit World(Ambientp, Camerap);

    HitRecord hit_object(Ray const&, double tmax = inf);
    bool hit_anything(Ray const&, double tmax = inf) const;
    bool v(Vec3 const&,  Vec3 const&) const;
};
