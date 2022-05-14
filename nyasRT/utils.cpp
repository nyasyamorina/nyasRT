#include "utils.h"
#include "setup.h"

#include <random>
#include <functional>
#include <png.h>
#ifdef RANDOM_WITH_TIME
#include <chrono>
#endif // RANDOM_WITH_TIME

using namespace std;
#ifdef RANDOM_WITH_TIME
using namespace std::chrono;
#endif // RANDOM_WITH_TIME


/* vec functions */

double abs(Vec2 const& v) {
    return sqrt(abs2(v));
}
double abs2(Vec2 const& v) {
    return dot(v, v);
}
double dot(Vec2 const& u, Vec2 const& v) {
    return u.x * v.x + u.y * v.y;
}
Vec2 normalize(Vec2 const& v) {
    return v / abs(v);
}
void absnorm(Vec2 & v, double & d) {
    d = abs(v);
    v /= d;
}
Vec2 spherical(double r, double phi) {
    return Vec2(r * cos(phi), r * sin(phi));
}
double to_spherical(Vec2 const& v, double & phi) {
    phi = atan2(v.y, v.x);
    return abs(v);
}

double abs(Vec3 const& v) {
    return sqrt(abs2(v));
}
double abs2(Vec3 const& v) {
    return dot(v, v);
}
double dot(Vec3 const& u, Vec3 const& v) {
    return u.x * v.x + u.y * v.y + u.z * v.z;
}
Vec3 cross(Vec3 const& u, Vec3 const& v) {
    return Vec3(u.y * v.z - u.z * v.y,
                u.z * v.x - u.x * v.z,
                u.x * v.y - u.y * v.x);
}
Vec3 normalize(Vec3 const& v) {
    return v / abs(v);
}
void absnorm(Vec3 & v, double & d) {
    d = abs(v);
    v /= d;
}
Vec3 spherical(double r, double theta, double phi) {
    auto r_st = r * sin(theta);
    return Vec3(r_st * cos(phi), r_st * sin(phi), r * cos(theta));
}
double to_spherical(Vec3 const& v, double & theta, double & phi) {
    auto r = abs(v);
    theta = acos(v.z / r);
    phi = atan2(v.y, v.x);
    return r;
}
Vec3 corrnormal(Vec3 const& n, Vec3 const& o) {
    return (dot(n, o) > 0.0) ? -n : n;
}
Vec3 reflect(Vec3 const& n, Vec3 const& i) {
    return i - 2.0 * dot(n, i) * n;
}


/* random functions */

#ifdef RANDOM_WITH_TIME
static auto _random_gen = mt19937_64(system_clock::now().time_since_epoch().count());
#else
static auto _random_gen = mt19937_64(0);
#endif // NOT_RANDOM_WITH_TIME

uint64_t randint() {
    return _random_gen();
}
uint64_t randint(uint64_t b) {
    auto dist = uniform_int_distribution((uint64_t)0, b - 1);
    return dist(_random_gen);
}
uint64_t randint(uint64_t a, uint64_t b) {
    auto dist = uniform_int_distribution(a, b - 1);
    return dist(_random_gen);
}
double rand01() {
    const auto dist = uniform_real_distribution(0.0, 1.0);
    return dist(_random_gen);
}


/* deg <-> rad */

double deg2rad(double deg) {
    constexpr double _2_rad = 3.141592653589793238462643383279502884 / 180;
    return deg * _2_rad;
}
double rad2deg(double rad) {
    constexpr double _2_deg = 180 / 3.141592653589793238462643383279502884;
    return rad * _2_deg;
}


/******************************************************************************
**********************************  Vec2  *************************************
******************************************************************************/


Vec2::Vec2()
    : x(0.0), y(0.0) {}
Vec2::Vec2(double s)
    : x(s), y(s) {}
Vec2::Vec2(double x_, double y_)
    : x(x_), y(y_) {}

Vec2 & Vec2::operator +=(double s) {
    this->x += s;
    this->y += s;
    return *this;
}
Vec2 & Vec2::operator -=(double s) {
    this->x -= s;
    this->y -= s;
    return *this;
}
Vec2 & Vec2::operator *=(double s) {
    this->x *= s;
    this->y *= s;
    return *this;
}
Vec2 & Vec2::operator /=(double s) {
    this->x /= s;
    this->y /= s;
    return *this;
}

Vec2 & Vec2::operator +=(Vec2 const& v) {
    this->x += v.x;
    this->y += v.y;
    return *this;
}
Vec2 & Vec2::operator -=(Vec2 const& v) {
    this->x -= v.x;
    this->y -= v.y;
    return *this;
}
Vec2 & Vec2::operator *=(Vec2 const& v) {
    this->x *= v.x;
    this->y *= v.y;
    return *this;
}
Vec2 & Vec2::operator /=(Vec2 const& v) {
    this->x /= v.x;
    this->y /= v.y;
    return *this;
}

Vec2 operator +(Vec2 const& v) {
    return v;
}
Vec2 operator -(Vec2 const& v) {
    return Vec2(-v.x, -v.y);
}

Vec2 operator +(Vec2 const& v, double s) {
    return Vec2(v.x + s, v.y + s);
}
Vec2 operator -(Vec2 const& v, double s) {
    return Vec2(v.x - s, v.y - s);
}
Vec2 operator *(Vec2 const& v, double s) {
    return Vec2(v.x * s, v.y * s);
}
Vec2 operator /(Vec2 const& v, double s) {
    return Vec2(v.x / s, v.y / s);
}

Vec2 operator +(double s, Vec2 const& v) {
    return Vec2(s + v.x, s + v.y);
}
Vec2 operator -(double s, Vec2 const& v) {
    return Vec2(s - v.x, s - v.y);
}
Vec2 operator *(double s, Vec2 const& v) {
    return Vec2(s * v.x, s * v.y);
}
Vec2 operator /(double s, Vec2 const& v) {
    return Vec2(s / v.x, s / v.y);
}

Vec2 operator +(Vec2 const& u, Vec2 const& v) {
    return Vec2(u.x + v.x, u.y + v.y);
}
Vec2 operator -(Vec2 const& u, Vec2 const& v) {
    return Vec2(u.x - v.x, u.y - v.y);
}
Vec2 operator *(Vec2 const& u, Vec2 const& v) {
    return Vec2(u.x * v.x, u.y * v.y);
}
Vec2 operator /(Vec2 const& u, Vec2 const& v) {
    return Vec2(u.x / v.x, u.y / v.y);
}

ostream & operator <<(ostream & o, Vec2 const & v) {
    return o << "v[" << v.x << ' ' << v.y << ']';
}


/******************************************************************************
**********************************  Vec3  *************************************
******************************************************************************/

Vec3::Vec3()
    : x(0.0), y(0.0), z(0.0) {}
Vec3::Vec3(double s)
    : x(s), y(s), z(s) {}
Vec3::Vec3(double x_, double y_, double z_)
    : x(x_), y(y_), z(z_) {}

Vec3 & Vec3::operator +=(double s) {
    this->x += s;
    this->y += s;
    this->z += s;
    return *this;
}
Vec3 & Vec3::operator -=(double s) {
    this->x -= s;
    this->y -= s;
    this->z -= s;
    return *this;
}
Vec3 & Vec3::operator *=(double s) {
    this->x *= s;
    this->y *= s;
    this->z *= s;
    return *this;
}
Vec3 & Vec3::operator /=(double s) {
    this->x /= s;
    this->y /= s;
    this->z /= s;
    return *this;
}

Vec3 & Vec3::operator +=(Vec3 const& v) {
    this->x += v.x;
    this->y += v.y;
    this->z += v.z;
    return *this;
}
Vec3 & Vec3::operator -=(Vec3 const& v) {
    this->x -= v.x;
    this->y -= v.y;
    this->z -= v.z;
    return *this;
}
Vec3 & Vec3::operator *=(Vec3 const& v) {
    this->x *= v.x;
    this->y *= v.y;
    this->z *= v.z;
    return *this;
}
Vec3 & Vec3::operator /=(Vec3 const& v) {
    this->x /= v.x;
    this->y /= v.y;
    this->z /= v.z;
    return *this;
}

Vec3 operator +(Vec3 const& v) {
    return v;
}
Vec3 operator -(Vec3 const& v) {
    return Vec3(-v.x, -v.y, -v.z);
}

Vec3 operator +(Vec3 const& v, double s) {
    return Vec3(v.x + s, v.y + s, v.z + s);
}
Vec3 operator -(Vec3 const& v, double s) {
    return Vec3(v.x - s, v.y - s, v.z - s);
}
Vec3 operator *(Vec3 const& v, double s) {
    return Vec3(v.x * s, v.y * s, v.z * s);
}
Vec3 operator /(Vec3 const& v, double s) {
    return Vec3(v.x / s, v.y / s, v.z / s);
}

Vec3 operator +(double s, Vec3 const& v) {
    return Vec3(s + v.x, s + v.y, s + v.z);
}
Vec3 operator -(double s, Vec3 const& v) {
    return Vec3(s - v.x, s - v.y, s - v.z);
}
Vec3 operator *(double s, Vec3 const& v) {
    return Vec3(s * v.x, s * v.y, s * v.z);
}
Vec3 operator /(double s, Vec3 const& v) {
    return Vec3(s / v.x, s / v.y, s / v.z);
}

Vec3 operator +(Vec3 const& u, Vec3 const& v) {
    return Vec3(u.x + v.x, u.y + v.y, u.z + v.z);
}
Vec3 operator -(Vec3 const& u, Vec3 const& v) {
    return Vec3(u.x - v.x, u.y - v.y, u.z - v.z);
}
Vec3 operator *(Vec3 const& u, Vec3 const& v) {
    return Vec3(u.x * v.x, u.y * v.y, u.z * v.z);
}
Vec3 operator /(Vec3 const& u, Vec3 const& v) {
    return Vec3(u.x / v.x, u.y / v.y, u.z / v.z);
}

ostream & operator <<(ostream & o, Vec3 const & v) {
    return o << "v[" << v.x << ' ' << v.y << ' ' << v.z << ']';
}


/******************************************************************************
**********************************  RGB  **************************************
******************************************************************************/

RGB::RGB()
    : r(0.0f), g(0.0f), b(0.0f) {}
RGB::RGB(float gray)
    : r(gray), g(gray), b(gray) {}
RGB::RGB(float r_, float g_, float b_)
    : r(r_), g(g_), b(b_) {}

RGB & RGB::operator +=(float s) {
    this->r += s;
    this->g += s;
    this->b += s;
    return *this;
}
RGB & RGB::operator -=(float s) {
    this->r -= s;
    this->g -= s;
    this->b -= s;
    return *this;
}
RGB & RGB::operator *=(float s) {
    this->r *= s;
    this->g *= s;
    this->b *= s;
    return *this;
}
RGB & RGB::operator /=(float s) {
    this->r /= s;
    this->g /= s;
    this->b /= s;
    return *this;
}

RGB & RGB::operator +=(RGB const& color) {
    this->r += color.r;
    this->g += color.g;
    this->b += color.b;
    return *this;
}
RGB & RGB::operator -=(RGB const& color) {
    this->r -= color.r;
    this->g -= color.g;
    this->b -= color.b;
    return *this;
}
RGB & RGB::operator *=(RGB const& color) {
    this->r *= color.r;
    this->g *= color.g;
    this->b *= color.b;
    return *this;
}
RGB & RGB::operator /=(RGB const& color) {
    this->r /= color.r;
    this->g /= color.g;
    this->b /= color.b;
    return *this;
}

RGB operator +(RGB const& color) {
    return color;
}
RGB operator -(RGB const& color) {
    return RGB(-color.r, -color.g, -color.b);
}

RGB operator +(RGB const& color, float s) {
    return RGB(color.r + s, color.g + s, color.b + s);
}
RGB operator -(RGB const& color, float s) {
    return RGB(color.r - s, color.g - s, color.b - s);
}
RGB operator *(RGB const& color, float s) {
    return RGB(color.r * s, color.g * s, color.b * s);
}
RGB operator /(RGB const& color, float s) {
    return RGB(color.r / s, color.g / s, color.b / s);
}

RGB operator +(float s, RGB const& color) {
    return RGB(s + color.r, s + color.g, s + color.b);
}
RGB operator -(float s, RGB const& color) {
    return RGB(s - color.r, s - color.g, s - color.b);
}
RGB operator *(float s, RGB const& color) {
    return RGB(s * color.r, s * color.g, s * color.b);
}
RGB operator /(float s, RGB const& color) {
    return RGB(s / color.r, s / color.g, s / color.b);
}

RGB operator +(RGB const& color1, RGB const& color2) {
    return RGB(color1.r + color2.r, color1.g + color2.g, color1.b + color2.b);
}
RGB operator -(RGB const& color1, RGB const& color2) {
    return RGB(color1.r - color2.r, color1.g - color2.g, color1.b - color2.b);
}
RGB operator *(RGB const& color1, RGB const& color2) {
    return RGB(color1.r * color2.r, color1.g * color2.g, color1.b * color2.b);
}
RGB operator /(RGB const& color1, RGB const& color2) {
    return RGB(color1.r / color2.r, color1.g / color2.g, color1.b / color2.b);
}

ostream & operator <<(ostream & o, RGB const & color) {
    return o << "rgb[" << color.r << ' ' << color.g << ' ' << color.b << ']';
}


/******************************************************************************
**********************************  Ray  **************************************
******************************************************************************/

Ray::Ray(Vec3 const& p, Vec3 const& d)
    : point(p), direction(d) {}

Vec3 Ray::at(double t) const {
    return this->point + t * this->direction;
}

ostream & operator <<(ostream & o, Ray const& ray) {
    return o << '(' << ray.point << " -> " << ray.direction << ')';
}


/******************************************************************************
********************************  HitRecord  **********************************
******************************************************************************/

HitRecord::HitRecord()
    : hit(false), t(numeric_limits<double>::infinity()), object_idx(0) {}


/******************************************************************************
********************************  LocalCoord  *********************************
******************************************************************************/

LocalCoord::LocalCoord(Vec3 const& n)
    : LocalCoord(n, (fabs(n.x) < 1e-8) ? Vec3(0, 1, 0) : Vec3(1, 0, 0)) {}
LocalCoord::LocalCoord(Vec3 const& n, Vec3 const& u_)
    : w(n), v(cross(n, u_)) {
    this->v /= abs(this->v);
    this->u = cross(this->v, n);
    this->u /= abs(this->u);
}

Vec3 LocalCoord::at(Vec3 const& p) const {
    return p.x * this->u + p.y * this->v + p.z * this->w;
}


/******************************************************************************
*********************************  Buffer2D  **********************************
******************************************************************************/

template class Buffer2D<RGB>;
template class Buffer2D<uint8_t>;
template class Buffer2D<Vec2>;
template class Buffer2D<Vec3>;

template<typename T> Buffer2D<T>::Buffer2D()
    : _data(nullptr), _rows(0), _cols(0) {}
template<typename T> Buffer2D<T>::Buffer2D(uint64_t n_rows, uint64_t n_cols)
    : _rows(n_rows), _cols(n_cols) {
    this->_alloc_buff();
}

template<typename T> Buffer2D<T>::Buffer2D(Buffer2D<T> const& other)
    : Buffer2D() {
    this->_copy_from(other);
}
template<typename T> Buffer2D<T>::Buffer2D(Buffer2D<T> && other) noexcept
    : Buffer2D() {
    swap(this->_data, other._data);
    swap(this->_rows, other._rows);
    swap(this->_cols, other._cols);
}
template<typename T> Buffer2D<T> & Buffer2D<T>::operator =(Buffer2D<T> const& other) {
    this->_copy_from(other);
    return *this;
}
template<typename T> Buffer2D<T> & Buffer2D<T>::operator =(Buffer2D<T> && other) noexcept {
    swap(this->_data, other._data);
    swap(this->_rows, other._rows);
    swap(this->_cols, other._cols);
    return *this;
}

template<typename T> Buffer2D<T>::~Buffer2D() {
    this->_dealloc_buff();
}

template<typename T> bool Buffer2D<T>::inbounds(uint64_t y, uint64_t x) const {
    return y < this->_rows && x < this->_cols;
}
template<typename T> uint64_t Buffer2D<T>::get_rows() const {
    return this->_rows;
}
template<typename T> uint64_t Buffer2D<T>::get_cols() const {
    return this->_cols;
}

template<typename T> T & Buffer2D<T>::operator ()(uint64_t y, uint64_t x) {
    return this->_data[y][x];
}
template<typename T> T const& Buffer2D<T>::operator ()(uint64_t y, uint64_t x) const {
    return this->_data[y][x];
}
template<typename T> T ** Buffer2D<T>::data_ptr() {
    return this->_data;
}
template<typename T> T const* const* Buffer2D<T>::data_ptr() const {
    return this->_data;
}

template<typename T> void Buffer2D<T>::_alloc_buff() {
    this->_data = new T *[this->_rows];
    T ** rows = this->_data;
    for (uint64_t y = 0; y < this->_rows; ++y) {
        *(rows++) = new T[this->_cols];
    }
}
template<typename T> void Buffer2D<T>::_dealloc_buff() {
    if (this->_data == nullptr) {
        return;
    }
    T ** rows = this->_data;
    for (uint64_t y = 0; y < this->_rows; ++y) {
        delete[] *(rows++);
    }
    delete[] this->_data;
    this->_data = nullptr;
}
template<typename T> void Buffer2D<T>::_copy_from(Buffer2D<T> const& other) {
    this->_dealloc_buff();
    this->_rows = other._rows;
    this->_cols = other._cols;
    this->_alloc_buff();
    T ** this_rows = this->_data;
    T ** other_rows = other._data;
    for (uint64_t y = 0; y < other._rows; ++y) {
        T * this_eles = *(this_rows++);
        T * other_eles = *(other_rows++);
        for (uint64_t x = 0; x < other._cols; ++x) {
            *(this_eles++) = *(other_eles++);
        }
    }
}


/******************************************************************************
***********************************  Image  ***********************************
******************************************************************************/

Image::Image()
    : _buffer() {}
Image::Image(uint64_t height, uint64_t width) {
    constexpr uint64_t png_pixels_max = PNG_SIZE_MAX / 24;
    if (height > png_pixels_max / width) {
        fprintf(stderr, "Image size is too large: size(%zi, %zi)", height, width);
    }
    else {
        this->_buffer = Buffer2D<RGB>(height, width);
    }
}

Image::Image(uint64_t height, uint64_t width, RGB const & color)
    : Image(height, width) {
    height = this->_buffer.get_rows();
    width = this->_buffer.get_cols();
    for (uint64_t y = 0; y < height; ++y) {
        for (uint64_t x = 0; x < width; ++x) {
            this->_buffer(y, x) = color;
        }
    }
}

void Image::clear() {
    this->clear(RGB(0.0f));
}
void Image::clear(RGB const& color) {
    auto rows = this->_buffer.data_ptr();
    auto height = this->get_height(), width = this->get_width();
    for (uint64_t y = 0; y < height; ++y) {
        auto pixels = *(rows++);
        for (uint64_t x = 0; x < width; ++x) {
            *(pixels++) = color;
        }
    }
}

bool Image::inbounds(uint64_t y, uint64_t x) const {
    return this->_buffer.inbounds(y, x);
}
uint64_t Image::get_height() const {
    return this->_buffer.get_rows();
}
uint64_t Image::get_width() const {
    return this->_buffer.get_cols();
}

RGB & Image::operator ()(uint64_t y, uint64_t x) {
    return this->_buffer(y, x);
}
RGB const& Image::operator ()(uint64_t y, uint64_t x) const {
    return this->_buffer(y, x);
}

bool Image::save_to(char const* filepath) const {
    auto saved = false;
    FILE * fp = nullptr;
    png_struct * png_ptr = nullptr;
    png_info * info_ptr = nullptr;
    uint8_t * row = nullptr;
    RGB const* const* this_rows;
    uint64_t height = this->get_height(), width = this->get_width();

    /* initialize workers */
    fopen_s(&fp, filepath, "wb");
    if (fp == nullptr) {
        fprintf(stderr, "[write png file] Cannot open or create file: %s", filepath);
        goto _image_save_return;
    }
    png_ptr = png_create_write_struct(PNG_LIBPNG_VER_STRING, nullptr, nullptr, nullptr);
    if (png_ptr == nullptr) {
        fprintf(stderr, "[write png file] Cannot create write struct");
        goto _image_save_return;
    }
    info_ptr = png_create_info_struct(png_ptr);
    if (info_ptr == nullptr) {
        fprintf(stderr, "[write png file] Cannot create info struct");
        goto _image_save_return;
    }
    if (setjmp(png_jmpbuf(png_ptr))) {
        fprintf(stderr, "[write png file] Error during initialize I/O");
        goto _image_save_return;
    }
    png_init_io(png_ptr, fp);

    /* write header */
    if (setjmp(png_jmpbuf(png_ptr))) {
        fprintf(stderr, "[write png file] Error during writing header");
        goto _image_save_return;
    }
    png_set_IHDR(png_ptr, info_ptr, (uint32_t)width, (uint32_t)height, 8, PNG_COLOR_TYPE_RGB,
                 PNG_INTERLACE_NONE, PNG_COMPRESSION_TYPE_BASE, PNG_FILTER_TYPE_BASE);
    png_write_info(png_ptr, info_ptr);

    /* write bytes */
    this_rows = this->_buffer.data_ptr();
    row = new uint8_t[(uint64_t)width * 3];
    for (uint64_t y = 0; y < height; ++y) {
        auto pixels_buff = row;
        auto this_pixels = *(this_rows++);
        for (uint64_t x = 0; x < width; ++x) {
            auto const& pixel = *(this_pixels++);
            *(pixels_buff++) = (uint8_t) (255.0f * pixel.r);
            *(pixels_buff++) = (uint8_t) (255.0f * pixel.g);
            *(pixels_buff++) = (uint8_t) (255.0f * pixel.b);
        }
        png_write_row(png_ptr, row);
    }

    /* end of write */
    png_write_end(png_ptr, info_ptr);
    saved = true;

_image_save_return:
    if (png_ptr != nullptr) {
        png_destroy_write_struct(&png_ptr, &info_ptr);
    }
    if (fp != nullptr) {
        fclose(fp);
    }
    delete[] row;
    return saved;
}

Image::Image(char const * filepath)
    : Image() {
    FILE * fp = nullptr;
    uint8_t sig_header[8];
    png_struct * png_ptr = nullptr;
    png_info * info_ptr = nullptr;
    RGB ** this_rows = nullptr, * this_pixels = nullptr;
    uint8_t ** im_rows = nullptr, * im_bytes = nullptr;
    uint32_t height, width;
    int32_t bit_depth, color_type, interlace_type;
    Buffer2D<uint8_t> im;
    size_t row_bytes, bytes_per_pixel;
    bool valid = true;

    /* initialize workers */
    fopen_s(&fp, filepath, "rb");
    if (fp == nullptr) {
        fprintf(stderr, "[read png file] Connot open file: %s", filepath);
        goto _image_load_return;
    }
    fread(sig_header, sizeof(char), 8, fp);
    if (png_sig_cmp(sig_header, 0, 8)) {
        fprintf(stderr, "[read png file] %s is not a png file", filepath);
        goto _image_load_return;
    }
    png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, nullptr, nullptr, nullptr);
    if (png_ptr == nullptr) {
        fprintf(stderr, "[read png file] Cannot create read struct");
        goto _image_load_return;
    }
    info_ptr = png_create_info_struct(png_ptr);
    if (info_ptr == nullptr) {
        fprintf(stderr, "[read png file] Cannot create info struct");
        goto _image_load_return;
    }
    if (setjmp(png_jmpbuf(png_ptr))) {
        fprintf(stderr, "[read png file] Error during initialize I/O");
        goto _image_load_return;
    }
    png_init_io(png_ptr, fp);
    png_set_sig_bytes(png_ptr, 8);

    /* load info */
    png_read_info(png_ptr, info_ptr);
    png_get_IHDR(png_ptr, info_ptr, &width, &height, &bit_depth,
                 &color_type, &interlace_type, nullptr, nullptr);

    /* set read method */
    png_set_scale_16(png_ptr);
    png_set_packing(png_ptr);
    if (color_type == PNG_COLOR_TYPE_PALETTE) {
        png_set_palette_to_rgb(png_ptr);
    }
    else if (color_type == PNG_COLOR_TYPE_GRAY && bit_depth < 8) {
        png_set_expand_gray_1_2_4_to_8(png_ptr);
    }

    /* read image */
    png_read_update_info(png_ptr, info_ptr);
    row_bytes = png_get_rowbytes(png_ptr, info_ptr);
    bytes_per_pixel = row_bytes / width;
    if (bytes_per_pixel != 3 && bytes_per_pixel != 4) {
        fprintf(stderr, "[read png file] Unprocessable color type (code: %i)", color_type);
        goto _image_load_return;
    }
    im = Buffer2D<uint8_t>(height, (uint64_t) row_bytes);
    png_read_image(png_ptr, im.data_ptr());

    /* end of read */
    png_read_end(png_ptr, info_ptr);

    /* byte color to float color */
    this->_buffer = Buffer2D<RGB>(height, width);
    this_rows = this->_buffer.data_ptr();
    im_rows = im.data_ptr();
    for (uint64_t y = 0; y < height; ++y) {
        this_pixels = *(this_rows++);
        im_bytes = *(im_rows++);
        for (uint64_t x = 0; x < width; ++x) {
            RGB & pixel = *(this_pixels++);
            uint8_t * im_bytep = im_bytes + bytes_per_pixel * x;
            pixel.r = 255.0f * *(im_bytep++);
            pixel.g = 255.0f * *(im_bytep++);
            pixel.b = 255.0f * *(im_bytep++);
        }
    }

_image_load_return:
    if (png_ptr != nullptr) {
        png_destroy_read_struct(&png_ptr, &info_ptr, nullptr);
    }
    if (fp != nullptr) {
        fclose(fp);
    }
}
