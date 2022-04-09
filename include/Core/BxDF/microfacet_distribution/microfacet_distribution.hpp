#pragma once

#include <Hinae/Vector3.hpp>
#include <Hinae/Point2.hpp>

using namespace Hinae;
using Spectrum = Vector3f;

struct microfacet_distribution
{
    virtual ~microfacet_distribution() = default;

    static f32 roughness_to_alpha(f32 roughness);

    virtual f32 D(const Vector3f& wh) const = 0;

    virtual f32 lambda(const Vector3f& w) const = 0;

    f32 G1(const Vector3f& w) const;

    f32 G(const Vector3f& wi, const Vector3f& wo) const;

    virtual std::tuple<Spectrum, Vector3f, f32>
    sample_f(const Vector3f& wi, const Point2f& u) const = 0;

    virtual f32 PDF(const Vector3f& wi, const Vector3f& wh) const = 0;
};
