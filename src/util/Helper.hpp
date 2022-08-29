#pragma once

#include <cassert>

#include "range.hpp"
#include "View.hpp"

template <typename T>
struct Helper
{
    T*  ptr;
    u32 size = 0;
    u32 max_size;

    auto add(const T& element) -> Helper&
    {
        assert(size <= max_size);
        ptr[size] = element;
        size += 1;
        return *this;
    }

    auto back() const -> T
    {
        assert(size != 0);
        return ptr[size - 1];
    }

    auto next_ptr() const -> T*
    {
        assert(size != 0);
        return &ptr[size];
    }

    auto operator [] (usize index) const -> T
    {
        assert(index <= max_size);
        return ptr[index];
    }

    auto operator [] (usize index) -> T&
    {
        assert(index <= max_size);
        return ptr[index];
    }

    auto clean() -> Helper&
    {
        for (auto i : range(size))
        {
            delete ptr[i];
        }
        return *this;
    }

    auto view() const -> View<T>
    {
        return {ptr, size};
    }
};
