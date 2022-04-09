#pragma once

#include <Hinae/Vector3.hpp>
#include <Hinae/Point2.hpp>

using namespace Hinae;

using Spectrum = Vector3f;

struct BxDF;
struct hit_record;

struct BSDF
{
private:
    BxDF* bxdf;
    Vector3f t, b, n;

    Vector3f to_world(const Vector3f& w) const;

    Vector3f to_local(const Vector3f& w) const;

public:
    BSDF() = default;

    BSDF(const hit_record& record, BxDF* bxdf);

    Spectrum f(const Vector3f& wi, const Vector3f& wo) const;

    std::tuple<Spectrum, Vector3f, f32>
    sample_f(const Vector3f& wi, const Point2f& p) const;
};
