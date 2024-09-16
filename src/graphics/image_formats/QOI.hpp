#pragma once

#include "Formatter.hpp"


namespace nyasRT
{
namespace image_formats
{
class QOI final : public Formatter
{
private:

    static constexpr inline u32 running_index(RGB24 c, u8 alpha) noexcept
    {
        return (3 * c.r + 5 * c.g + 7 * c.b + 11 * alpha) & 63;
    }
    static constexpr inline u32 running_index(RGB24 c) noexcept
    {
        return running_index(c, 255);
    }

    static constexpr inline void diff(RGB24 & d, RGB24 c) noexcept
    {
        d.r = c.r - d.r + static_cast<u8>(2);
        d.g = c.g - d.g + static_cast<u8>(2);
        d.b = c.b - d.b + static_cast<u8>(2);
    }
    static constexpr inline bool can_encode_diff(RGB24 c) noexcept
    {
        return (c.r < 4) && (c.g < 4) && (c.b < 4);
    }

    static constexpr inline void luma(RGB24 & c) noexcept
    {
        c.r += static_cast<u8>(8) - c.g;
        c.b += static_cast<u8>(8) - c.g;
        c.g +=  static_cast<u8>(30);
    }
    static constexpr inline bool can_encode_luma(RGB24 c) noexcept
    {
        return (c.r < 16) && (c.g < 64) && (c.b < 16);
    }


    u8 header[14];
    u8 index;
    RGB24 running[64];
    RGB24 prev, curr;

public:

    QOI() noexcept
    : header{}, index{0}, running{}, prev{}, curr{} {
        // initialize header
        header[0] = 'q'; header[1] = 'o'; header[2] = 'i'; header[3] = 'f';
        header[12] = 3; header[13] = 0;

        // the alpha of colors in `running` are initialized to 0,
        // but we don't track the alphas, so its necessary to make sure that
        // in the first case of `curr = Black`, `curr == running[index]` must false.
        VEC_CONSTEXPR RGB24 Black24 = RGB24(0,   0, 0);
        VEC_CONSTEXPR RGB24 Green24 = RGB24(0, 255, 0);
        //assert(running_index(Black24) != running_index(Green24));
        running[running_index(Black24)] = Green24;
    }

    virtual bool write(std::ostream & file, GraphicsBuffer const& gbuf) override
    {
        auto write = [&] (void const* data, u64 length)
        {
            file.write(reinterpret_cast<char const*>(data), length);
        };

        // big-endian
        *reinterpret_cast<u32 *>(header + 4) = std::byteswap( gbuf.width());
        *reinterpret_cast<u32 *>(header + 8) = std::byteswap(gbuf.height());
        write(header, 14);

        RGB const* pix = gbuf.begin();
        RGB const* end = gbuf.end();

        // although the previous of the first pixel should be RGBA(0),
        // but we don't track the alphas, so the first pixel must be not qual to
        // the previous one, even cannot close to it.
        prev = RGB24(0); //RGB_to_RGB24(*pix);
        //prev.r = ~prev.r; prev.g = ~prev.g; prev.b = ~prev.b;
        //prev.r += 90; prev.g += 90; prev.b += 90;

        u8 encoded;
        while (pix < end)
        {
            curr = RGB_to_RGB24(*(pix++));
            index = running_index(curr);

            if (curr == prev)                                   // -> QOI_OP_RUN
            {
                encoded = 0xC0;
                while ((encoded < 0xFC) && (pix < end))
                {
                    if (curr == RGB_to_RGB24(*pix))
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

    virtual bool read(std::istream & file, GraphicsBuffer & gbuf) override
    {
        auto read = [&] (void * data, u64 length) -> bool
        {
            file.read(reinterpret_cast<char *>(data), length);
            return file.gcount() == length;
        };

        size2_t fig_size;
        if (!read(header, 14)) { return false; }
        fig_size.x = std::byteswap(*reinterpret_cast<u32 *>(header + 4));
        fig_size.y = std::byteswap(*reinterpret_cast<u32 *>(header + 8));
        if (fig_size != gbuf.size()) { gbuf = GraphicsBuffer(fig_size); }

        RGB * pix = gbuf.begin();
        RGB * end = gbuf.end();

        prev = RGB24(0, 0, 0);
        u8 alpha = (header[12] == 3) ? 255 : 0;

        u8 encoded, flag, data;
        while (pix < end)
        {
            if (!read(&encoded, 1)) { return false; }
            flag = encoded & 0xC0; data = encoded & 0x3F;

            if (flag == 0x00)               // -> QOI_OP_INDEX
            {
                curr = running[data];
                // running with alpha ???
            }
            else if (flag == 0x40)          // -> QOI_OP_DIFF
            {
                curr.r = prev.r - u8(2) + (data >> 4);
                curr.g = prev.g - u8(2) + ((data >> 2) & 0x3);
                curr.b = prev.b - u8(2) + (data & 0x3);
            }
            else if (flag == 0x80)          // -> QOI_OP_LUMA
            {
                if (!read(&encoded, 1)) { return false; }
                curr.g = data   - u8(32);
                curr.r = prev.r - u8(8) + curr.g + (encoded >>  4);
                curr.b = prev.b - u8(8) + curr.g + (encoded & 0xF);
                curr.g += prev.g;
            }
            else
            {
                if (encoded == 0xFE)        // -> QOI_OP_RGB
                {
                    if (!read(&curr, 3)) { return false; }
                    alpha = 255;
                }
                else if (encoded == 0xFF)   // -> QOI_OP_RGBA
                {
                    if (!read(&curr, 3)) { return false; }
                    if (!read(&alpha, 1)) { return false; }
                }
                else                        // -> QOI_OP_RUN
                {
                    RGB prev_color = RGB24_to_RGB(prev);
                    while ((data--) > 0)
                    {
                        *(pix++) = prev_color;
                        if (!(pix < end)) { return true; }
                    }
                }
            }

            *(pix++) = RGB24_to_RGB(curr);
            index = running_index(curr, alpha);
            prev = running[index] = curr;
        }
        return true;
    }
};

} // namespace Formatters
} // namespace nyasRT

