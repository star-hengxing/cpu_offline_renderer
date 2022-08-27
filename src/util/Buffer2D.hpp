#pragma once

#include <cassert>

#include <algorithm>
#include <vector>
#include <tuple>

using usize = std::size_t;

template <typename T>
struct Buffer2D
{
protected:
    std::vector<T> data;
    usize width = 0, height = 0;

public:
    Buffer2D() = default;
    Buffer2D(usize width, usize height) : width(width), height(height), data(width * height) {}

    Buffer2D(const Buffer2D& other)
        : data(other.data)
        , width(other.width), height(other.height) {}

    Buffer2D& operator = (const Buffer2D& other)
    {
        auto tmp{other};
        tmp.swap(*this);
        return *this;
    }

    Buffer2D(Buffer2D&& other)
        : data(std::move(other.data))
        , width(other.width), height(other.height) {}

    Buffer2D& operator = (Buffer2D&& other)
    {
        auto tmp{std::move(other)};
        tmp.swap(*this);
        return *this;
    }

    void fill(const T& value) noexcept
    {
        std::ranges::fill(data, value);
    }

    void resize(usize width, usize height) noexcept
    {
        data.resize(width * height);
        this->width = width;
        this->height = height;
    }

    auto raw() noexcept
    {
        return data.data();
    }

    std::tuple<usize, usize> get_width_height() const noexcept
    {
        return {width, height};
    }
    
    auto begin() noexcept
    {
        return data.begin();
    }
    
    auto end() noexcept
    {
        return data.end();
    }

    const auto cbegin() const noexcept
    {
        return data.cbegin();
    }

    const auto cend() const noexcept
    {
        return data.cend();
    }

    void swap(Buffer2D& other)
    {
        data.swap(other.data);
        std::swap(width, other.width);
        std::swap(height, other.height);
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
