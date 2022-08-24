#pragma once

#include "Vector3.hpp"
#include "Point3.hpp"

template <arithmetic T>
constexpr std::tuple<Vector3<T>, Vector3<T>>
local_coordinate_system(const Vector3<T>& v1)
{
    const auto [x, y, z] = v1;
    Vector3<T> v2;
    if(std::abs(x) > std::abs(y))
        v2 = Vector3<T>{-z, ZERO<T>, x} / std::sqrt(x * x + z * z);
    else
        v2 = Vector3<T>{ZERO<T>, z, -y} / std::sqrt(y * y + z * z);
    return {v2, cross(v1, v2)};
}

template <arithmetic T>
constexpr Point3<T> cartesian_to_spherical(const Point3<T>& p)
{
    const auto v = as<Vector3, T>(p);
    const auto [x, y, z] = v;

    const T radius = v.norm();
    const T theta  = std::acos(z / radius);
    const T phi    = std::atan2(y, x);
    return {radius, theta, phi};
}

template <arithmetic T>
constexpr Point3<T> spherical_to_cartesian(const Point3<T>& p)
{
    const auto [r, theta, phi] = p;
    return
    {
        r * std::sin(theta) * std::cos(phi),
        r * std::sin(theta) * std::sin(phi),
        r * std::cos(theta)
    };
}

template <arithmetic T>
constexpr Vector3<T> local_to_world(
    const Vector3<T>& t, const Vector3<T>& b, const Vector3<T>& n, const Vector3<T>& dir)
{
    const auto [x, y, z] = dir;
    return
    {
        t.x * x + b.x * y + n.x * z,
        t.y * x + b.y * y + n.y * z,
        t.z * x + b.z * y + n.z * z
    };
}

template <arithmetic T>
constexpr Vector3<T> local_to_world(const Vector3<T>& n, const Vector3<T>& dir)
{
    const auto [t, b] = local_coordinate_system(n);
    return local_to_world(t, b, n, dir);
}

template <arithmetic T>
constexpr Vector3<T> world_to_local(
    const Vector3<T>& t, const Vector3<T>& b, const Vector3<T>& n, const Vector3<T>& dir)
{
    return {dot(dir, t), dot(dir, b), dot(dir, n)};
}

template <arithmetic T>
constexpr Vector3<T> world_to_local(const Vector3<T>& n, const Vector3<T>& dir)
{
    const auto [t, b] = local_coordinate_system(n);
    return world_to_local(t, b, n, dir);
}
