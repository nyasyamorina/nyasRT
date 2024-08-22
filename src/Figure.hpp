#pragma once

#include <bit>
#include <filesystem>
#include <fstream>
#include <functional>
#include <memory>

#include "utils.hpp"
#include "geometry/vec2.hpp"
#include "RGB.hpp"


class Figure
{
protected:

    u32 _height, _width;
    RGB * _pixels;

    RGB * _new_pixel_array()
    {
        u64 t = total();
        return (t != 0) ? (new RGB[t]) : nullptr;
    }

public:

    Figure(u32 height_, u32 width_)
    : _height(height_), _width(width_), _pixels(_new_pixel_array()) {}
    Figure(u32 height_, u32 width_, RGB const& c)
    : Figure(height_, width_) {
        for (RGB & rgb : (*this))
        {
            rgb = c;
        }
    }

    Figure(Figure const& fig)
    : _height(fig._height), _width(fig._width), _pixels(_new_pixel_array()) {
        std::memcpy(_pixels, fig._pixels, sizeof(RGB) * total());
    }
    Figure(Figure && fig) noexcept
    : _height(std::exchange(fig._height, 0)), _width(std::exchange(fig._width, 0)), _pixels(std::exchange(fig._pixels, nullptr)) {}

    ~Figure() noexcept
    {
        delete _pixels;
    }

    Figure & operator = (Figure const& fig)
    {
        if (this != &fig)
        {
            delete _pixels;

            _height = fig._height;
            _width  = fig._width;
            _pixels = _new_pixel_array();
            std::memcpy(_pixels, fig._pixels, sizeof(RGB) * total());
        }
        return *this;
    }
    Figure & operator = (Figure && fig) noexcept
    {
        if (this != &fig)
        {
            delete _pixels;

            _height = std::exchange(fig._height, 0);
            _width  = std::exchange(fig._width, 0);
            _pixels = std::exchange(fig._pixels, nullptr);
        }
        return *this;
    }


    /******** iterators & member access ********/

    u32 height() const noexcept
    {
        return _height;
    }
    u32 width() const noexcept
    {
        return _width;
    }
    u64 total() const noexcept
    {
        return static_cast<u64>(_height) * static_cast<u64>(_width);
    }
    fg aspect_ratio() const noexcept
    {
        return static_cast<fg>(_width) / static_cast<fg>(_height);
    }

    RGB * begin() noexcept
    {
        return _pixels;
    }
    RGB * end() noexcept
    {
        return _pixels + total();
    }
    RGB const* begin() const noexcept
    {
        return _pixels;
    }
    RGB const* end() const noexcept
    {
        return _pixels + total();
    }

    RGB & operator () (u64 linear_index) noexcept
    {
        return _pixels[linear_index];
    }
    RGB & operator () (u32 row, u32 col) noexcept
    {
        return _pixels[row + col * _width];
    }
    RGB const& operator () (u64 linear_index) const noexcept
    {
        return _pixels[linear_index];
    }
    RGB const& operator () (u32 row, u32 col) const noexcept
    {
        return _pixels[row + col * _width];
    }

    vec2g pixel_size() const noexcept
    {
        return vec2g(static_cast<fg>(2) / _width, static_cast<fg>(2) / _height);
    }
    vec2g screen_position(u32 row, u32 col) const noexcept
    {
        fg x = row,      y = col;
        x *= 2;          y *= -2;
        x -= _width - 1; y += _height - 1;
        x /= _width;     y /= _height;
        return vec2g(x, y);
    }

    /******** save to file ********/

    template<class Saver> bool save(std::filesystem::path const& path) const
    {
        std::fstream file(path, std::ios::out | std::ios::binary | std::ios::trunc);
        if (!file.is_open()) { return false; }
        Saver saver(file);
        return saver(*this);
    }
};

using FigurePtr = std::shared_ptr<Figure>;
using FigureConstPtr = std::shared_ptr<Figure const>;


class QOI final
{
private:

    class RGB24
    {
    public:

        u8 r, g, b;

        CONST_FUNC RGB24()
        : RGB24(defaults<RGB>::Black) {}
        CONST_FUNC RGB24(u8 r_, u8 g_, u8 b_)
        : r(r_), g(g_), b(b_) {}
        CONST_FUNC RGB24(RGB const& c)
        : r(clamp01(c.r) * 255.0f + 0.5f), g(clamp01(c.g) * 255.0f + 0.5f), b(clamp01(c.b) * 255.0f + 0.5f) {}

        CONST_FUNC RGB24 & operator = (RGB const& c)
        {
            r = clamp01(c.r) * 255.0f + 0.5f;
            g = clamp01(c.g) * 255.0f + 0.5f;
            b = clamp01(c.b) * 255.0f + 0.5f;
            return *this;
        }

        CONST_FUNC auto operator <=> (RGB24 const&) const = default;
        CONST_FUNC auto operator <=> (RGB const& c) const
        {
            return (*this) <=> RGB24(c);
        }

        CONST_FUNC u8 running_index() const
        {
            return (3 * r + 5 * g + 7 * b + (11 * 255/*a*/)) & 0x3F;
        }

        CONST_FUNC RGB24 & diff(RGB24 c)
        {
            r = c.r - r + 2;
            g = c.g - g + 2;
            b = c.b - b + 2;
            return *this;
        }
        CONST_FUNC bool can_encode_diff() const
        {
            return (r < 4) && (g < 4) && (b < 4);
        }

        CONST_FUNC RGB24 & luma()
        {
            r += 8 - g;
            b += 8 - g;
            g += 30;
            return *this;
        }
        CONST_FUNC bool can_encode_luma() const
        {
            return (r < 16) && (g < 64) && (b < 16);
        }
    };

    std::fstream & file;
    u8 header[14];
    u8 index;
    RGB24 running[64];
    RGB24 prev, curr;

public:

    QOI(std::fstream & file_) noexcept
    : file(file_), header{}, index{0}, running{}, prev{}, curr{} {
        // initialize header
        header[0] = 'q'; header[1] = 'o'; header[2] = 'i'; header[3] = 'f';
        header[12] = 3; header[13] = 1;

        // the alpha of colors in `running` are initialized to 0,
        // but we don't track the alphas, so its necessary to make sure that
        // in the first case of `curr = Black`, `curr == running[index]` must false.
        constexpr RGB24 Black24 = RGB24(0,   0, 0);
        constexpr RGB24 Green24 = RGB24(0, 255, 0);
        static_assert(Black24.running_index() != Green24.running_index());
        running[Black24.running_index()] = Green24;
    }

    void write(void const* data, u64 length)
    {
        file.write((char *)(data), length);
    }

    bool operator () (Figure const& fig)
    {
        // big-endian
        *reinterpret_cast<u32 *>(header + 4) = std::byteswap( fig.width());
        *reinterpret_cast<u32 *>(header + 8) = std::byteswap(fig.height());
        write(header, 14);

        RGB const* pix = fig.begin();
        RGB const* end = fig.end();

        // although the previous of the first pixel should be RGBA(0),
        // but we don't track the alphas, so the first pixel must be not qual to
        // the previous one, even cannot close to it.
        prev = RGB24(pix->r + 90, pix->g + 90, pix->b + 90);

        u8 encoded;
        while (pix < end)
        {
            curr = *(pix++);
            index = curr.running_index();

            if (curr == prev)                                   // -> QOI_OP_RUN
            {
                encoded = 0xC0;
                while ((encoded < 0xFC) && (pix < end))
                {
                    if (curr == *pix)
                    {
                        encoded++;
                        pix++;
                    }
                    else { break; }
                }
                write(&encoded, 1);
            }

            else if (curr == running[index])                    // -> QOI_OP_INDEX
            {
                write(&index, 1);
            }

            else {
                if (prev.diff(curr); prev.can_encode_diff())    // -> QOI_OP_DIFF
                {
                    encoded = 0x40 | (prev.r << 4) | (prev.g << 2) | prev.b;
                    write(&encoded, 1);
                }

                else if (prev.luma(); prev.can_encode_luma())   // -> QOI_OP_LUMA
                {
                    encoded = 0x80 | prev.g;
                    write(&encoded, 1);
                    encoded = (prev.r << 4) | prev.b;
                    write(&encoded, 1);
                }

                else                                            // -> QOI_OP_RGB
                {
                    encoded = 0xFE;
                    write(&encoded, 1);
                    write(&curr, 3);
                }
            }
            prev = running[index] = curr;
        }

        // end_of_file
        constexpr u64 bswap1 = std::byteswap<u64>(1);
        *reinterpret_cast<u64 *>(header) = bswap1;
        write(header, sizeof(u64));

        file.flush();
        return true;
    }
};
