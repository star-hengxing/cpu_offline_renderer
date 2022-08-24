#pragma once

#include "basic.hpp"

using Point4f = Point4<f32>;
using Point4d = Point4<f64>;
using Point4i = Point4<isize>;

template <arithmetic T>
struct Point4
{
    T x, y, z, w;
    
    constexpr Point4(T v) : x(v), y(v), z(v), w(w) {}
    constexpr Point4(T x, T y, T z, T w) : x(x), y(y), z(z), w(w) {}
    constexpr Point4(const Point3<T>& p, T w = ONE<T>) : x(p.x), y(p.y), z(p.z), w(w) {}

    Point4() = default;
    auto operator <=> (const Point4<T>&) const = default;

    T operator [] (usize i) const
    { 
        assert(i <= 3);
        return (&x)[i];
    }
    
    T& operator [] (usize i)
    { 
        assert(i <= 3);
        return (&x)[i];
    }

    T operator [] (Axis axis) const
    { 
        return (&x)[static_cast<usize>(axis)];
    }

    T& operator [] (Axis axis)
    { 
        return (&x)[static_cast<usize>(axis)];
    }
};

template <arithmetic T>
std::ostream& operator << (std::ostream& os, const Point4<T>& v)
{
    return os << std::make_tuple(v.x, v.y, v.z, v.w);
}
