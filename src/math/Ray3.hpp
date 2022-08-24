#pragma once

#include "Vector3.hpp"
#include "Point3.hpp"

using Ray3f = Ray3<f32>;
using Ray3d = Ray3<f64>;
using Ray3i = Ray3<isize>;

template <arithmetic T>
struct Ray3
{
    Point3<T> origin;
    Vector3<T> direction;

    constexpr Ray3() = default;
    constexpr Ray3(const Point3<T>& origin, const Vector3<T>& direction)
        : origin(origin)
        , direction(direction) {}

    constexpr Point3<T> at(T t) const { return origin + direction * t; }
    
    template <Axis axis>
    constexpr T inv_at(T v) const
    {
        return (v - origin[axis]) / direction[axis];
    }

    template <Axis axis>
    constexpr T at(T t) const
    {
        return origin[axis] + direction[axis] * t;
    }
};

template <arithmetic T>
std::ostream& operator << (std::ostream& os, const Ray3<T>& ray)
{
    return os << std::make_tuple(ray.origin, ray.direction);
}
