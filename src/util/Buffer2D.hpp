#pragma once

#include <cassert>

#include <algorithm>
#include <vector>
#include <tuple>

using usize = std::size_t;

template <typename T, typename Container = std::vector<T>>
struct Buffer2D
{
protected:
    usize width = 0, height = 0;
    Container data;

public:
    Buffer2D() = default;
    Buffer2D(usize width, usize height) : width(width), height(height), data(width * height) {}

    Buffer2D(const Buffer2D&) = delete;
    Buffer2D& operator = (const Buffer2D&) = delete;

    Buffer2D(Buffer2D&&) = default;
    Buffer2D& operator = (Buffer2D&&) = default;

    void fill(const T& value) noexcept
    {
        std::ranges::fill(data, value);
    }

    void copy(const Buffer2D& buf) noexcept
    {
        data = buf.data;
    }

    Buffer2D clone() const noexcept
    {
        Buffer2D ret{width, height};
        ret.copy(*this);
        return ret;
    }

    const T* raw() const noexcept
    {
        return data.data();
    }

    Container& get() noexcept
    {
        return data;
    }

    std::tuple<usize, usize> get_width_height() const noexcept
    {
        return {width, height};
    }

    struct ProxyIndex
    {
        Buffer2D& buffer;
        usize index;

        ProxyIndex(Buffer2D& buf, usize width) : buffer(buf), index(width)
        {
            assert(width < buffer.width);
        }

        operator T () const
        {
            return buffer.data[index];
        }

        ProxyIndex& operator [] (usize height)
        {
            assert(height < buffer.height);
            index = height * buffer.width + index;
            return *this;
        }

        const T& operator [] (usize height) const
        {
            assert(height < buffer.width);
            return buffer.data[height * buffer.width + index];
        }

        ProxyIndex& operator = (const T& value)
        {
            buffer.data[index] = value;
            return *this;
        }
    };

    ProxyIndex operator [] (usize index)
    {
        return ProxyIndex(*this, index);
    }

    const ProxyIndex operator [] (usize index) const
    {
        return ProxyIndex(const_cast<Buffer2D&>(*this), index);
    }
};
