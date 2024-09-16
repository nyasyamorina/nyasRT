#pragma once

#include <filesystem>
#include <fstream>
#include <iostream>

#include "../../common.hpp"
#include "../GraphicsBuffer.hpp"


namespace nyasRT
{
namespace image_formats
{
class Formatter
{
public:

    bool save(std::filesystem::path path, GraphicsBuffer const& gbuf)
    {
        std::ofstream file(path, std::ios::out | std::ios::binary | std::ios::trunc);
        if (!file.is_open()) { return false; }
        return write(file, gbuf);
    }
    bool load(std::filesystem::path path, GraphicsBuffer & gbuf)
    {
        std::ifstream file(path, std::ios::in | std::ios::binary);
        if (!file.is_open()) { return false; }
        return read(file, gbuf);
    }

    virtual bool write(std::ostream & file, GraphicsBuffer const& gbuf) = 0;
    virtual bool read(std::istream & file, GraphicsBuffer & gbuf) = 0;
};

} // namespace Formatters
} // namespace nyasRT
