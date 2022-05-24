#pragma once

#include <memory>


//#define RANDOM_WITH_TIME
#define USE_SHIRLEY_CIRCLE_MAPPING
//#define LOAD_SHOW_SAMPLE_FUNCTIONS


/* basic types */

struct Vec3;
struct Vec2;
struct RGB;
struct Ray;
struct LocalCoord;

struct HitRecord;

template<typename T> class Buffer2D;
class Image;


/* sample types */

class Generator;
template<typename V> class SampleType;
typedef std::shared_ptr<Generator> Generatorp;

struct SamplerArgs;
template<typename ST> class Sampler;
template<typename ST> using Samplerp = std::shared_ptr<Sampler<ST>>;


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

class World;
