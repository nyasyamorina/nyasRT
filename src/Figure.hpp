#pragma once

#include <cassert>
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

    vec2<u32> _size;
    RGB * _pixels;

    RGB * _new_pixel_array()
    {
        u64 t = total();
        return (t != 0) ? (new RGB[t]) : nullptr;
    }

public:

    Figure(vec2<u32> size_)
    : _size(size_), _pixels(_new_pixel_array()) {}
    Figure(vec2<u32> size_, RGB const& c)
    : Figure(size_) {
        for (RGB & rgb : (*this)) { rgb = c; }
    }

    Figure(Figure const& fig)
    : _size(fig._size), _pixels(_new_pixel_array()) {
        std::memcpy(_pixels, fig._pixels, sizeof(RGB) * total());
    }
    Figure(Figure && fig) noexcept
    : _size(std::exchange(fig._size, defaults<vec2<u32>>::O)), _pixels(std::exchange(fig._pixels, nullptr)) {}

    ~Figure() noexcept
    {
        delete _pixels;
    }

    Figure & operator = (Figure const& fig)
    {
        if (this != &fig)
        {
            delete _pixels;
            _size = fig._size;
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
            _size = std::exchange(fig._size, defaults<vec2<u32>>::O);
            _pixels = std::exchange(fig._pixels, nullptr);
        }
        return *this;
    }


    /******** iterators & member access ********/

    u32 width() const noexcept
    {
        return _size.x;
    }
    u32 height() const noexcept
    {
        return _size.y;
    }
    vec2<u32> size() const noexcept
    {
        return _size;
    }
    u64 total() const noexcept
    {
        return prod(vec2<u64>(_size));
    }
    fg aspect_ratio() const noexcept
    {
        return static_cast<fg>(_size.x) / static_cast<fg>(_size.y);
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

    RGB & operator [] (u64 linear_index) noexcept
    {
        return _pixels[linear_index];
    }
    RGB & operator [] (vec2<u32> index) noexcept
    {
        return _pixels[static_cast<u64>(index.x) + static_cast<u64>(index.y) * _size.x];
    }
    RGB const& operator [] (u64 linear_index) const noexcept
    {
        return _pixels[linear_index];
    }
    RGB const& operator [] (vec2<u32> index) const noexcept
    {
        return _pixels[static_cast<u64>(index.x) + static_cast<u64>(index.y) * _size.x];
    }

    vec2g pixel_size() const noexcept
    {
        return 2 / vec2g(_size);
    }
    vec2g screen_position(vec2<u32> index) const noexcept
    {
        vec2g pos = index;
        pos.mul(2).sub(_size).add(1).div(_size);
        pos *= defaults<vec2g>::X - defaults<vec2g>::Y;
        return pos;
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


class ViewedFigure
{
public:

    class Iterator final
    {
    public:

        ViewedFigure & view;
        vec2<u32> index;
        RGB * pixel_p;

        Iterator(ViewedFigure & view_) noexcept
        : view{view_}, index{view.start}, pixel_p{&view.figure[index]} {}

        RGB & operator * () noexcept
        {
            return *pixel_p;
        }
        RGB & operator -> () noexcept
        {
            return *pixel_p;
        }

        Iterator & operator ++ () noexcept
        {
            index.x++;
            pixel_p++;
            if (index.x == view.stop.x)
            {
                index.x = view.start.x;
                index.y++;
                pixel_p += view.figure.size().x - (view.stop.x - view.start.x);
            }
            return *this;
        }
        Iterator & operator ++ (i32) noexcept
        {
            return ++(*this);
        }

        bool operator < (Iterator const& other) const noexcept
        {
            return pixel_p < other.pixel_p;
        }
    };


    Figure & figure;
    vec2<u32> start, stop;

    ViewedFigure(Figure & fig) noexcept
    : ViewedFigure(fig, defaults<vec2<u32>>::O, fig.size()) {}
    ViewedFigure(Figure & fig, vec2<u32> start_, vec2<u32> stop_) noexcept
    : figure{fig}, start{start_}, stop{stop_} {}

    ViewedFigure & set_start(vec2<i32> new_start, vec2<u32> size) noexcept
    {
        stop = min(figure.size(), new_start + size);
        start = max(0, new_start);
        return *this;
    }

    Iterator begin() noexcept
    {
        return Iterator(*this);
    }
    Iterator end() noexcept
    {
        Iterator result(*this);
        result.index = stop - defaults<vec2<u32>>::Y;
        result.pixel_p = &figure[result.index];
        return result;
    }
};


class QOI final
{
private:

    static CONST_FUNC u32 running_index(RGB24 c) noexcept
    {
        return 3 * c.r + 5 * c.g + 7 * c.b + (11 * 255/*a*/) & 63;
    }

    static CONST_FUNC void diff(RGB24 & d, RGB24 c) noexcept
    {
        d.r = c.r - d.r + static_cast<u8>(2);
        d.g = c.g - d.g + static_cast<u8>(2);
        d.b = c.b - d.b + static_cast<u8>(2);
    }
    static CONST_FUNC bool can_encode_diff(RGB24 c) noexcept
    {
        return (c.r < 4) && (c.g < 4) && (c.b < 4);
    }

    static CONST_FUNC void luma(RGB24 & c) noexcept
    {
        c.r += static_cast<u8>(8) - c.g;
        c.b += static_cast<u8>(8) - c.g;
        c.g +=  static_cast<u8>(30);
    }
    static CONST_FUNC bool can_encode_luma(RGB24 c) noexcept
    {
        return (c.r < 16) && (c.g < 64) && (c.b < 16);
    }


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
        static_assert(running_index(Black24) != running_index(Green24));
        running[running_index(Black24)] = Green24;
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
            index = running_index(curr);

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
                if (diff(prev, curr); can_encode_diff(prev))    // -> QOI_OP_DIFF
                {
                    encoded = 0x40 | (prev.r << 4) | (prev.g << 2) | prev.b;
                    write(&encoded, 1);
                }

                else if (luma(prev); can_encode_luma(prev))     // -> QOI_OP_LUMA
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
