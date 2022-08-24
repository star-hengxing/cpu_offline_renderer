#pragma once

#include <optional>
#include <complex>

#include <util/marco.hpp>
#include "Trigonometric.hpp"

template <arithmetic T>
constexpr Vector3<T> reflect(const Vector3<T>& wi, const Vector3<T>& n)
{
    return wi - 2 * dot(wi, n) * n;
}

template <arithmetic T>
constexpr std::optional<Vector3<T>>
refract(const Vector3<T>& wi, const Vector3<T>& n, T eta)
{
    const T cos_theta_i  = dot(wi, n);
    const T sin2_theta_i = cos_to_sin2(cos_theta_i);
    const T sin2_theta_t = eta * eta * sin2_theta_i;

    if (sin2_theta_t >= 1) return {};

    const T cos_theta_t = sin2_to_cos(sin2_theta_t);
    return Vector3(eta * wi - (eta * cos_theta_i + cos_theta_t) * n);
}

NAMESPACE_BEGIN(Fresnel)

template <bool safe = true, arithmetic T>
constexpr T dielectric(T cos_theta_i, T eta_i, T eta_t)
{
    if constexpr (safe)
    {
        if (cos_theta_i < 0)
        {
            std::swap(eta_i, eta_t);
            cos_theta_i = -cos_theta_i;
        }
    }

    T sin_theta_i = sin_to_cos(cos_theta_i);
    T sin_theta_t = eta_i / eta_t * sin_theta_i;
    if (sin_theta_t >= 1) return 1;

    T cos_theta_t = sin_to_cos(sin_theta_t);

    T r_parl = ((eta_t * cos_theta_i) - (eta_i * cos_theta_t))
               / ((eta_t * cos_theta_i) + (eta_i * cos_theta_t));

    T r_perp = ((eta_i * cos_theta_i) - (eta_t * cos_theta_t))
               / ((eta_i * cos_theta_i) + (eta_t * cos_theta_t));

    return (pow2(r_parl) + pow2(r_perp)) / 2;
}

template <bool safe = true, arithmetic T>
constexpr T conductor(T cos_theta_i, std::complex<T> eta_i, std::complex<T> eta_t)
{
    if constexpr (safe)
    {
        if (cos_theta_i < 0)
        {
            std::swap(eta_i, eta_t);
            cos_theta_i = -cos_theta_i;
        }
    }

    T    sin_theta_i = std::sqrt(ONE<T> - cos_theta_i * cos_theta_i);
    auto sin_theta_t = eta_i / eta_t * sin_theta_i;

    auto cos_theta_t = std::sqrt(ONE<T> - sin_theta_t * sin_theta_t);

    auto r_parl = ((eta_t * cos_theta_i) - (eta_i * cos_theta_t))
                  / ((eta_t * cos_theta_i) + (eta_i * cos_theta_t));

    auto r_perp = ((eta_i * cos_theta_i) - (eta_t * cos_theta_t))
                  / ((eta_i * cos_theta_i) + (eta_t * cos_theta_t));

    return (std::norm(r_parl) + std::norm(r_perp)) / 2;
}

NAMESPACE_END(Fresnel)
