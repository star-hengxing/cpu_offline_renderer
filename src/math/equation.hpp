#pragma once

#include <optional>

#include "basic_type.hpp"

template <arithmetic T>
std::optional<std::tuple<T, T>> quadratic(T a, T b, T c)
{
    T det = b * b - 4 * a * c;
    if(det < 0) return std::nullopt;
    det = std::copysign(std::sqrt(det), b);
    T tmp = (b + det) / -2;
    T x1 = c / tmp;
    T x2 = tmp / a;
    return std::make_optional(std::make_tuple(x1, x2));
}
