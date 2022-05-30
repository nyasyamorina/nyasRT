#pragma once

#include <numeric>
#include <iostream>
#include "setup.h"

constexpr double inf = std::numeric_limits<double>::infinity();



/* vec functions */

double abs(Vec2 const&);
double abs2(Vec2 const&);
double dot(Vec2 const&, Vec2 const&);
Vec2 normalize(Vec2 const&);
double absnorm(Vec2 &);
Vec2 spherical(double r, double phi);
double to_spherical(Vec2 const&, double & phi);

double abs(Vec3 const&);
double abs2(Vec3 const&);
double dot(Vec3 const&, Vec3 const&);
Vec3 cross(Vec3 const&, Vec3 const&);
Vec3 normalize(Vec3 const&);
double absnorm(Vec3 &);
Vec3 spherical(double r, double theta, double phi);
double to_spherical(Vec3 const&, double & theta, double & phi);
Vec3 corrnormal(Vec3 const& n, Vec3 const& o);
Vec3 reflect(Vec3 const& n, Vec3 const& i);

Vec3 correct_viewup(Vec3 const&, double);
Vec3 correct_viewup(double, double, double);


/* random functions */

uint64_t randint();
uint64_t randint(uint64_t);
uint64_t randint(uint64_t, uint64_t);
double rand01();


/* deg <=> rad */

double deg2rad(double);
double rad2deg(double);



/******************************************************************************
**********************************  Vec2  *************************************
******************************************************************************/

struct Vec2 final {
public:
    double x, y;

    Vec2();
    explicit Vec2(double);
    explicit Vec2(double, double);

    Vec2(Vec2 const&) = default;
    Vec2(Vec2 &&) = default;
    Vec2 & operator =(Vec2 const&) = default;
    Vec2 & operator =(Vec2 &&) = default;

    Vec2 & operator +=(double);
    Vec2 & operator -=(double);
    Vec2 & operator *=(double);
    Vec2 & operator /=(double);

    Vec2 & operator +=(Vec2 const&);
    Vec2 & operator -=(Vec2 const&);
    Vec2 & operator *=(Vec2 const&);
    Vec2 & operator /=(Vec2 const&);
};

Vec2 operator +(Vec2 const&);
Vec2 operator -(Vec2 const&);

Vec2 operator +(Vec2 const&, double);
Vec2 operator -(Vec2 const&, double);
Vec2 operator *(Vec2 const&, double);
Vec2 operator /(Vec2 const&, double);

Vec2 operator +(double, Vec2 const&);
Vec2 operator -(double, Vec2 const&);
Vec2 operator *(double, Vec2 const&);
Vec2 operator /(double, Vec2 const&);

Vec2 operator +(Vec2 const&, Vec2 const&);
Vec2 operator -(Vec2 const&, Vec2 const&);
Vec2 operator *(Vec2 const&, Vec2 const&);
Vec2 operator /(Vec2 const&, Vec2 const&);

std::ostream & operator <<(std::ostream &, Vec2 const&);


/******************************************************************************
**********************************  Vec3  *************************************
******************************************************************************/

struct Vec3 final {
public:
    double x, y, z;

    Vec3();
    explicit Vec3(double);
    explicit Vec3(double, double, double);

    Vec3(Vec3 const&) = default;
    Vec3(Vec3 &&) = default;
    Vec3 & operator =(Vec3 const&) = default;
    Vec3 & operator =(Vec3 &&) = default;

    Vec3 & operator +=(double);
    Vec3 & operator -=(double);
    Vec3 & operator *=(double);
    Vec3 & operator /=(double);

    Vec3 & operator +=(Vec3 const&);
    Vec3 & operator -=(Vec3 const&);
    Vec3 & operator *=(Vec3 const&);
    Vec3 & operator /=(Vec3 const&);
};

Vec3 operator +(Vec3 const&);
Vec3 operator -(Vec3 const&);

Vec3 operator +(Vec3 const&, double);
Vec3 operator -(Vec3 const&, double);
Vec3 operator *(Vec3 const&, double);
Vec3 operator /(Vec3 const&, double);

Vec3 operator +(double, Vec3 const&);
Vec3 operator -(double, Vec3 const&);
Vec3 operator *(double, Vec3 const&);
Vec3 operator /(double, Vec3 const&);

Vec3 operator +(Vec3 const&, Vec3 const&);
Vec3 operator -(Vec3 const&, Vec3 const&);
Vec3 operator *(Vec3 const&, Vec3 const&);
Vec3 operator /(Vec3 const&, Vec3 const&);

std::ostream & operator <<(std::ostream &, Vec3 const&);


/******************************************************************************
**********************************  RGB  **************************************
******************************************************************************/

struct RGB final {
public:
    float r, g, b;

    RGB();
    explicit RGB(float);
    explicit RGB(float, float, float);

    RGB(RGB const&) = default;
    RGB(RGB &&) = default;
    RGB & operator =(RGB const&) = default;
    RGB & operator =(RGB &&) = default;

    RGB & operator +=(float);
    RGB & operator -=(float);
    RGB & operator *=(float);
    RGB & operator /=(float);

    RGB & operator +=(RGB const&);
    RGB & operator -=(RGB const&);
    RGB & operator *=(RGB const&);
    RGB & operator /=(RGB const&);
};

RGB operator +(RGB const&);
RGB operator -(RGB const&);

RGB operator +(RGB const&, float);
RGB operator -(RGB const&, float);
RGB operator *(RGB const&, float);
RGB operator /(RGB const&, float);

RGB operator +(float, RGB const&);
RGB operator -(float, RGB const&);
RGB operator *(float, RGB const&);
RGB operator /(float, RGB const&);

RGB operator +(RGB const&, RGB const&);
RGB operator -(RGB const&, RGB const&);
RGB operator *(RGB const&, RGB const&);
RGB operator /(RGB const&, RGB const&);

std::ostream & operator <<(std::ostream &, RGB const&);


/******************************************************************************
**********************************  Ray  **************************************
******************************************************************************/

struct Ray final {
public:
    Vec3 point, direction;

    Ray();
    explicit Ray(Vec3 const& p, Vec3 const& d);

    Ray(Ray const&) = default;
    Ray(Ray &&) = default;
    Ray & operator =(Ray const&) = default;
    Ray & operator =(Ray &&) = default;

    Vec3 at(double t) const;
};

std::ostream & operator <<(std::ostream &, Ray const&);

/******************************************************************************
********************************  HitRecord  **********************************
******************************************************************************/

struct HitRecord final {
public:
    World & world;                  // world reference
    Ray ray;                        // currently calculating ray
    uint32_t depth;                 // depth of ray
    bool hit;                       // is hit an object?
    Object * object_p;              // hitted object pointer
    double t;                       // distance traveled by the ray
    Vec3 point;                     // hitted point on object
    Vec3 normal;                    // normal on hitted point
    Vec2 tex;                       // texture coord on hitted point

    explicit HitRecord(World &, Ray const&, uint32_t, double t_);

    HitRecord(HitRecord const&) = default;
    HitRecord(HitRecord &&) = default;
    HitRecord & operator =(HitRecord const&) = default;
    HitRecord & operator =(HitRecord &&) = default;

    void correct_normal();
    void set_values(Object &, double, Vec3 const& n, Vec2 const&);
};


/******************************************************************************
********************************  LocalCoord  *********************************
******************************************************************************/

struct LocalCoord final {
public:
    Vec3 u, v, w;

    LocalCoord(Vec3 const& n);
    LocalCoord(Vec3 const& n, Vec3 const& u_);

    LocalCoord(LocalCoord const&) = default;
    LocalCoord(LocalCoord &&) = default;
    LocalCoord & operator =(LocalCoord const&) = default;
    LocalCoord & operator =(LocalCoord &&) = default;

    Vec3 at(Vec3 const&) const;
};


/******************************************************************************
*********************************  Buffer2D  **********************************
******************************************************************************/

template<class T> class Buffer2D final {
public:
    typedef T eltype;

    Buffer2D();
    explicit Buffer2D(uint64_t n_rows, uint64_t n_cols);

    Buffer2D(Buffer2D<T> const&);
    Buffer2D(Buffer2D<T> &&) noexcept;
    Buffer2D<T> & operator =(Buffer2D<T> const&);
    Buffer2D<T> & operator =(Buffer2D<T> &&) noexcept;

    ~Buffer2D();

    bool inbounds(uint64_t y, uint64_t x) const;
    uint64_t get_rows() const;
    uint64_t get_cols() const;

    T & operator ()(uint64_t y, uint64_t x);
    T const& operator ()(uint64_t y, uint64_t x) const;
    T ** data_ptr();
    T const* const* data_ptr() const;

private:
    T ** _data;
    uint64_t _rows, _cols;

    void _alloc_buff();
    void _dealloc_buff();
    void _copy_from(Buffer2D<T> const&);
};


/******************************************************************************
***********************************  Image  ***********************************
******************************************************************************/

class Image final {
public:
    Image();
    explicit Image(uint64_t height, uint64_t width);
    explicit Image(uint64_t height, uint64_t width, RGB const&);
    explicit Image(char const* filepath);

    void clear();
    void clear(RGB const&);

    bool inbounds(uint64_t h, uint64_t w) const;
    uint64_t get_height() const;
    uint64_t get_width() const;

    RGB & operator ()(uint64_t h, uint64_t w);
    RGB const& operator ()(uint64_t h, uint64_t w) const;

    bool save_to(char const* filepath) const;

private:
    Buffer2D<RGB> _buffer;
};
