#pragma once

#include "basic_type.hpp"

template <typename T>
struct View
{
    T*    ptr;
    usize size;

    View() {}
    View(T* ptr, usize size) : ptr(ptr), size(size) {}

    auto empty() const
    {
        return size == 0;
    }

    auto operator [] (usize index) const
    {
        return ptr[index];
    }

    auto begin() const noexcept
    {
        return ptr;
    }

    auto end() const noexcept
    {
        return ptr + size;
    }
};
