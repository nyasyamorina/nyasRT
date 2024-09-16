#pragma once

#include <memory>

#include "../common.hpp"


namespace nyasRT
{
class GraphicsBuffer final
{
public:

    static inline RGB * new_buffer_array(size2_t buff_size)
    {
        u64 const total = u64(buff_size.x) * u64(buff_size.y);
        if (total == 0) { return nullptr; }
        return new RGB[total];
    }

private:

    size2_t _size;
    RGB * _data;

public:

    GraphicsBuffer() noexcept
    : _size(0, 0), _data{nullptr} {}
    GraphicsBuffer(size2_t size_)
    : _size{size_}, _data{new_buffer_array(size_)} {}

    GraphicsBuffer(GraphicsBuffer const& gbuf)
    : _size{gbuf._size}, _data{new_buffer_array(gbuf._size)} {
        ::std::memcpy(_data, gbuf._data, sizeof(RGB) * total());
    }
    GraphicsBuffer(GraphicsBuffer && gbuf) noexcept
    : _size{::std::exchange(gbuf._size, index2_t(0))}, _data{::std::exchange(gbuf._data, nullptr)} {}

    GraphicsBuffer & operator = (GraphicsBuffer const& gbuf)
    {
        if (this != &gbuf)
        {
            delete _data;
            _size = gbuf._size;
            _data = new_buffer_array(_size);
            std::memcpy(_data, gbuf._data, sizeof(RGB) * total());
        }
        return *this;
    }
    GraphicsBuffer & operator = (GraphicsBuffer && gbuf) noexcept
    {
        if (this != &gbuf)
        {
            delete _data;
            _size = std::exchange(gbuf._size, size2_t(0));
            _data = std::exchange(gbuf._data, nullptr);
        }
        return *this;
    }

    GraphicsBuffer & swap(GraphicsBuffer & gbuf) noexcept
    {
        if (this != &gbuf)
        {
            std::swap(_size, gbuf._size);
            std::swap(_data, gbuf._data);
        }
        return *this;
    }

    GraphicsBuffer & fill(RGB const& c) noexcept
    {
        RGB * const end = _data + total();
        for (RGB * pixel = _data; pixel < end; pixel++) { *pixel = c; }
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
    size2_t size() const noexcept
    {
        return _size;
    }
    u64 total() const noexcept
    {
        return u64(_size.x) * u64(_size.y);
    }
    fg aspect_ratio() const noexcept
    {
        return fg(_size.x) / fg(_size.y);
    }

    RGB * begin() noexcept
    {
        return _data;
    }
    RGB * end() noexcept
    {
        return _data + total();
    }
    RGB const* begin() const noexcept
    {
        return _data;
    }
    RGB const* end() const noexcept
    {
        return _data + total();
    }
    RGB * data() noexcept
    {
        return _data;
    }
    RGB const* data() const noexcept
    {
        return _data;
    }

    bool inrange(u64 linear_index) const noexcept
    {
        return linear_index < total();
    }
    bool inrange(index2_t index) const noexcept
    {
        return ((0 <= index.x) && (index.x < _size.x)) && ((0 <= index.y) && (index.y < _size.y));
    }

    RGB & operator[](u64 linear_index) noexcept
    {
        return _data[linear_index];
    }
    RGB & operator[](index2_t index) noexcept
    {
        return _data[u64(index.x) + u64(index.y) * _size.x];
    }
    RGB const& operator[](u64 linear_index) const noexcept
    {
        return _data[linear_index];
    }
    RGB const& operator[](index2_t index) const noexcept
    {
        return _data[u64(index.x) + u64(index.y) * _size.x];
    }

    vec2g pixel_size() const noexcept
    {
        vec2g const _size_g = _size;
        return fg(2) / _size_g;
    }
    vec2g position(index2_t index) const noexcept
    {
        vec2g const _size_g = _size;
        vec2g pos = index;
        pos = (pos * fg(2) - _size_g + fg(1)) / _size_g;
        pos.y *= -1;
        return pos;
    }
};


class SubBufferIterator;
class SubBufferConstIterator;

class SubBuffer final
{
private:

    GraphicsBuffer * _gbuf;
    index2_t _offset, _end;

public:

    explicit SubBuffer(GraphicsBuffer & gbuf_) noexcept
    : SubBuffer(gbuf_, index2_t(0), gbuf_.size()) {}
    SubBuffer(GraphicsBuffer & gbuf_, index2_t offset_, size2_t size_) noexcept
    : _gbuf{&gbuf_}, _offset{offset_}, _end{_offset + index2_t(size_)} {}

    SubBuffer(SubBuffer & sbuf_, index2_t offset_, size2_t size_) noexcept
    : _gbuf{sbuf_._gbuf}, _offset{offset_ + sbuf_._offset}, _end{_offset + index2_t(size_)} {}


    /******** getter & setter ********/

    GraphicsBuffer & buffer() noexcept
    {
        return *_gbuf;
    }
    GraphicsBuffer const& buffer() const noexcept
    {
        return *_gbuf;
    }
    index2_t offset() const noexcept
    {
        return _offset;
    }

    u32 width() const noexcept
    {
        return _end.x - _offset.x;
    }
    u32 height() const noexcept
    {
        return _end.y - _offset.y;
    }
    size2_t size() const noexcept
    {
        return size2_t(_end - _offset);
    }
    u64 total() const noexcept
    {
        return u64(width()) * u64(height());
    }
    fg aspect_ratio() const noexcept
    {
        return fg(width()) / fg(height());
    }

    RGB & operator[](u64 linear_index) noexcept
    {
        u64 const w = width();
        index2_t index;
        index.y = linear_index / w;
        index.x = linear_index % w;
        return (*this)[index];
    }
    RGB & operator[](index2_t index) noexcept
    {
        return (*_gbuf)[index + _offset];
    }
    RGB const& operator[](u64 linear_index) const noexcept
    {
        u64 const w = width();
        index2_t index;
        index.y = linear_index / w;
        index.x = linear_index % w;
        return (*this)[index];
    }
    RGB const& operator[](index2_t index) const noexcept
    {
        return (*_gbuf)[index + _offset];
    }

    bool inrange(u64 linear_index) const noexcept
    {
        return linear_index < total();
    }
    bool inrange(index2_t index) const noexcept
    {
        return ((0 <= index.x) && (index.x < width())) && ((0 <= index.y) && (index.y < height()));
    }

    SubBuffer & buffer(GraphicsBuffer & gbuf_) noexcept
    {
        _gbuf = &gbuf_;
        return *this;
    }
    SubBuffer & buffer(SubBuffer & sbuf_) noexcept
    {
        _gbuf = sbuf_._gbuf;
        _offset += sbuf_._offset;
        return *this;
    }
    SubBuffer & offset(index2_t offset_) noexcept
    {
        _offset = offset_;
        return *this;
    }
    SubBuffer & size(size2_t size_) noexcept
    {
        _end = _offset + index2_t(size_);
        return *this;
    }

    // clamp the size of SubBuffer to be within GraphicsBuffer
    SubBuffer & clamp() noexcept
    {
        index2_t const gbsize = _gbuf->size();
        _offset = ::nyasRT::clamp(_offset, index2_t(0), gbsize);
        _end    = ::nyasRT::clamp(_end   , index2_t(0), gbsize);
        return *this;
    }

    /******** iterators ********/

    SubBufferIterator begin() noexcept;
    SubBufferIterator end() noexcept;
    SubBufferConstIterator begin() const noexcept;
    SubBufferConstIterator end() const noexcept;
};


class SubBufferIterator final
{
private:

    SubBuffer * _sbuf;
    index2_t _local_index;
    RGB * _pixel;

public:

    explicit SubBufferIterator(SubBuffer & sbuf_) noexcept
    : _sbuf{&sbuf_}, _local_index{index2_t(0)}, _pixel{&(sbuf_[_local_index])} {}

    SubBufferIterator & operator*() noexcept
    {
        return *this;
    }

    RGB & pixel() const noexcept
    {
        return *_pixel;
    }
    // index in SubBuffer
    index2_t local_index() const noexcept
    {
        return _local_index;
    }
    // index in GraphicsBuffer
    index2_t global_index() const noexcept
    {
        return _local_index + _sbuf->offset();
    }

    bool operator==(SubBufferIterator const& other) const noexcept
    {
        if (_sbuf != other._sbuf) { return false; }
        // all out of range iterators are the same
        bool this_out_of_range  = !(_sbuf->inrange(_local_index));
        bool other_out_of_range = !(_sbuf->inrange(other._local_index));
        if (this_out_of_range || other_out_of_range) { return this_out_of_range && other_out_of_range; }
        return _local_index == other._local_index;
    }
    bool operator!=(SubBufferIterator const& other) const noexcept
    {
        return !((*this) == other);
    }

    SubBufferIterator & operator++() noexcept
    {
        u32 const w = _sbuf->width();
        _local_index.x++;
        _pixel++;
        if (_local_index.x == w)
        {
            _local_index.x = 0;
            _local_index.y++;
            _pixel = &((*_sbuf)[_local_index]);
        }
        return *this;
    }

    SubBufferIterator & operator+=(index2_t offset) noexcept
    {
        // used in `SubBuffer::end()` for leaving the range quickly
        _local_index += offset;
        _pixel = &((*_sbuf)[_local_index]);
        return *this;
    }
};
class SubBufferConstIterator final
{
private:

    SubBuffer const* _sbuf;
    index2_t _local_index;
    RGB const* _pixel;

public:

    explicit SubBufferConstIterator(SubBuffer const& sbuf_) noexcept
    : _sbuf{&sbuf_}, _local_index{index2_t(0)}, _pixel{&(sbuf_[_local_index])} {}

    SubBufferConstIterator & operator*() noexcept
    {
        return *this;
    }

    RGB const& pixel() const noexcept
    {
        return *_pixel;
    }
    // index in SubBuffer
    index2_t local_index() const noexcept
    {
        return _local_index;
    }
    // index in GraphicsBuffer
    index2_t global_index() const noexcept
    {
        return _local_index + _sbuf->offset();
    }

    bool operator==(SubBufferConstIterator const& other) const noexcept
    {
        if (_sbuf != other._sbuf) { return false; }
        // all out of range iterators are the same
        bool this_in_range  = _sbuf->inrange(_local_index);
        bool other_in_range = _sbuf->inrange(other._local_index);
        if (!this_in_range || !other_in_range) { return this_in_range ^ other_in_range; }
        return _local_index == other._local_index;
    }
    bool operator!=(SubBufferConstIterator const& other) const noexcept
    {
        return !((*this) == other);
    }

    SubBufferConstIterator & operator++() noexcept
    {
        u32 const w = _sbuf->width();
        _local_index.x++;
        _pixel++;
        if (_local_index.x == w)
        {
            _local_index.x = 0;
            _local_index.y++;
            _pixel = &((*_sbuf)[_local_index]);
        }
        return *this;
    }

    SubBufferConstIterator & operator+=(index2_t offset) noexcept
    {
        // used in `SubBuffer::end()` for leaving the range quickly
        _local_index += offset;
        _pixel = &((*_sbuf)[_local_index]);
        return *this;
    }
};

inline SubBufferIterator SubBuffer::begin() noexcept
{
    return SubBufferIterator(*this);
}
inline SubBufferIterator SubBuffer::end() noexcept
{
    SubBufferIterator iter = begin();
    iter += size();
    return iter;
}
inline SubBufferConstIterator SubBuffer::begin() const noexcept
{
    return SubBufferConstIterator(*this);
}
inline SubBufferConstIterator SubBuffer::end() const noexcept
{
    SubBufferConstIterator iter = begin();
    iter += size();
    return iter;
}

} // namespace naysRT
