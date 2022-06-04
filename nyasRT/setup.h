#pragma once

#include <stdint.h>
#include <memory>
#include <cmath>



/******************************************************************************
*********************************  Options  ***********************************
******************************************************************************/

//#define RANDOM_WITH_TIME
#define USE_SHIRLEY_CIRCLE_MAPPING
//#define LOAD_SHOW_SAMPLE_FUNCTIONS


/******************************************************************************
**********************************  types  ************************************
******************************************************************************/

/* basic types */

struct Vec3;
struct Vec2;
struct RGB;
struct Ray;
struct LocalCoord;

struct HitRecord;

template<class T> class Buffer2D;
class Image;

class RemapColor;


/* sample types */

class Generator;
template<class V> class SampleType;
typedef std::shared_ptr<Generator> Generatorp;

template<class ST> class Sampler;
template<class ST> using Samplerp = std::shared_ptr<Sampler<ST>>;


/* object types */

class Light;
typedef std::shared_ptr<Light> Lightp;

class BRDF;
typedef std::shared_ptr<BRDF> BRDFp;
class Texture;
typedef std::shared_ptr<Texture> Texturep;
class Material;
typedef std::shared_ptr<Material> Materialp;
class Object;
typedef std::shared_ptr<Object> Objectp;


/* world types */

class Ambient;
typedef std::shared_ptr<Ambient> Ambientp;
class Sky;
typedef std::shared_ptr<Sky> Skyp;

class Camera;
typedef std::shared_ptr<Camera> Camerap;

class RayTracer;
typedef std::shared_ptr<RayTracer> RayTracerp;

class World;
typedef std::shared_ptr<World> Worldp;
