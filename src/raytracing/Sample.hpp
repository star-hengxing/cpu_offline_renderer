#pragma once

#include <math/Trigonometric.hpp>
#include <math/Vector3.hpp>
#include <math/Vector2.hpp>
#include <math/Point3.hpp>
#include <math/Point2.hpp>

NAMESPACE_BEGIN(Sample)

template <arithmetic T>
Point2<T> uniform_disk(const Point2<T>& p)
{
    const auto radius = std::sqrt(p.x);
    const auto theta  = p.y * 2 * PI<T>;
    return {radius * std::cos(theta), radius * std::sin(theta)};
}

template <arithmetic T>
T uniform_disk_pdf(const Vector2<T>& v)
{
    return INV_PI<T>;
}

template <arithmetic T>
Point3<T> uniform_sphere(const Point2<T>& p)
{
    const auto theta = 2 * PI<T> * p.x;
    const auto phi = std::acos((1 - 2 * p.y));
    return
    {
        std::sin(phi) * std::cos(theta),
        std::sin(phi) * std::sin(theta),
        std::cos(phi)
    };
}

template <arithmetic T>
T uniform_sphere_pdf()
{
    return INV_4PI<T>;
}

template <arithmetic T>
Point3<T> uniform_hemisphere(const Point2<T>& p)
{
    const auto phi = 2 * PI<T> * p.x;
    const auto cos_theta = p.y;
    const auto sin_theta = cos_to_sin(cos_theta);
    return
    {
        sin_theta * std::cos(phi),
        sin_theta * std::sin(phi),
        cos_theta
    };
}

template <arithmetic T>
T uniform_hemisphere_pdf()
{
    return INV_2PI<T>;
}

template <arithmetic T>
Point3<T> cosine_hemisphere(const Point2<T>& p)
{
    const auto phi = 2 * PI<T> * p.x;
    const auto cos_theta = std::sqrt(p.y);
    const auto sin_theta = cos_to_sin(cos_theta);
    return
    {
        sin_theta * std::cos(phi),
        sin_theta * std::sin(phi),
        cos_theta
    };
}

template <arithmetic T>
T cosine_hemisphere_pdf(const Vector3<T>& v)
{
    return Local::cos_theta(v) * INV_PI<T>;
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
