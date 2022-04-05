#pragma once

#include <Hinae/Vector3.hpp>
#include <Hinae/Vector2.hpp>
#include <Hinae/Point3.hpp>
#include <Hinae/Point2.hpp>

using namespace Hinae;

NAMESPACE_BEGIN(Sample)

template <arithmetic T>
Point2<T> uniform_disk(const Point2<T>& p)
{
    const T radius = p.x;
    const T theta  = p.y * 2 * PI<T>;
    return {radius * std::cos(theta), radius * std::sin(theta)};
}

template <arithmetic T>
T uniform_disk_pdf(const Vector2<T>& v)
{
    // x^2 + y^2 <= 1
    return v.norm() <= ZERO<T> ? INV_PI<T> : ZERO<T>;
}

template <arithmetic T>
Point2<T> uniform_sphere(const Point2<T>& p)
{
    const T radius = p.x;
    const T theta  = p.y * 2 * PI<T>;
    return {radius * std::cos(theta), radius * std::sin(theta)};
}

template <arithmetic T>
T uniform_uniform_sphere_pdf()
{
    return INV_4PI<T>;
}

template <arithmetic T>
Point3<T> uniform_hemisphere(const Point2<T>& p)
{
    const T phi   = p.x * 2 * PI<T>;
    const T theta = std::acos(1 - p.y);
    return 
    {
        std::sin(theta) * std::cos(phi),
        std::sin(theta) * std::sin(phi),
        std::cos(theta)
    };
}

template <arithmetic T>
T uniform_hemisphere_pdf(const Vector3<T>& v)
{
    return v.z < ZERO<T> ? ZERO<T> : INV_2PI<T>;
}

template <arithmetic T>
Point3<T> cosine_hemisphere(const Point2<T>& p)
{
    const auto [x, y] = p;
    const T theta  = y * 2 * PI<T>;
    return
    {
        std::cos(theta) * std::sqrt(x),
        std::sin(theta) * std::sqrt(x),
        std::sqrt(ONE<T> - x)
    };
}

template <arithmetic T>
T cosine_hemisphere_pdf(const Vector3<T>& v)
{
    return v.z < ZERO<T> ? ZERO<T> : v.z * INV_PI<T>;
}

template <arithmetic T>
T Tent(T x)
{
    if(x < static_cast<T>(0.5))
        return std::sqrt(2 * x) - 1;
    else
        return 1 - std::sqrt(2 - 2 * x);
}

template <arithmetic T>
T Tent_pdf(T x)
{
    return x >= -1 && x <= 1 ? ONE<T> - std::abs(x) : ZERO<T>;
}

template <arithmetic T>
Point2<T> tent(const Point2<T>& p)
{
    return {Tent(p.x), Tent(p.y)};
}

template <arithmetic T>
T tent_pdf(const Vector2<T>& v)
{
    return Tent_pdf(v.x) * Tent_pdf(v.y);
}

NAMESPACE_END(Sample)
