#pragma once

#include <util/marco.hpp>
#include "Vector3.hpp"

template <arithmetic T>
constexpr T sin_to_cos2(T sin)
{
    return max(ZERO<T>, ONE<T> - sin * sin);
}

template <arithmetic T>
constexpr T cos_to_sin2(T cos)
{
    return max(ZERO<T>, ONE<T> - cos * cos);
}

template <arithmetic T>
constexpr T sin2_to_cos(T sin2)
{
    return std::sqrt(ONE<T> - sin2);
}

template <arithmetic T>
constexpr T cos2_to_sin(T cos2)
{
    return std::sqrt(ONE<T> - cos2);
}

template <arithmetic T>
constexpr T sin_to_cos(T sin)
{
    return std::sqrt(sin_to_cos2(sin));
}

template <arithmetic T>
constexpr T cos_to_sin(T cos)
{
    return std::sqrt(cos_to_sin2(cos));
}

NAMESPACE_BEGIN(Local)

template <arithmetic T>
constexpr T cos_theta(const Vector3<T>& v)
{
    return v.z;
}

template <arithmetic T>
constexpr T cos2_theta(const Vector3<T>& v)
{
    return cos_theta(v) * cos_theta(v);
}

template <arithmetic T>
constexpr T abs_cos_theta(const Vector3<T>& v)
{
    return std::abs(cos_theta(v));
}

template <arithmetic T>
constexpr T sin_theta(const Vector3<T>& v)
{
    return cos_to_sin(cos_theta(v));
}

template <arithmetic T>
constexpr T sin2_theta(const Vector3<T>& v)
{
    return cos_to_sin2(cos_theta(v));
}

template <arithmetic T>
constexpr T tan_theta(const Vector3<T>& v)
{
    return sin_theta(v) / cos_theta(v);
}

template <arithmetic T>
constexpr T tan2_theta(const Vector3<T>& v)
{
    return sin2_theta(v) / cos2_theta(v);
}

template <arithmetic T>
constexpr T cos_phi(const Vector3<T>& v)
{
    const T sin = sin_theta(v);
    return is_zero(sin) ? 1 : clamp(-ONE<T>, v.x / sin, ONE<T>);
}

template <arithmetic T>
constexpr T sin_phi(const Vector3<T>& v)
{
    const T sin = sin_theta(v);
    return is_zero(sin) ? 1 : clamp(-ONE<T>, v.y / sin, ONE<T>);
}

template <arithmetic T>
constexpr T cos2_phi(const Vector3<T>& v)
{
    return cos_phi(v) * cos_phi(v);
}

template <arithmetic T>
constexpr T sin2_phi(const Vector3<T>& v)
{
    return sin_phi(v) * sin_phi(v);
}

NAMESPACE_END(Local)
