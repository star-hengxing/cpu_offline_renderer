#pragma once

#include <optional>

#include <math/Vector3.hpp>
#include <math/Point2.hpp>

using Spectrum = Vector3f;

struct BxDF;
struct hit_record;

struct bsdf_sample
{
    Spectrum f;
    Vector3f wo;
    f32 pdf;
    bool is_specular;
};

struct BSDF
{
private:
    BxDF* bxdf = nullptr;
    Vector3f t, b, n;

    Vector3f to_world(const Vector3f& w) const;

    Vector3f to_local(const Vector3f& w) const;

public:
    BSDF() = default;

    BSDF(const hit_record& record, BxDF* bxdf);

    Spectrum f(const Vector3f& wi, const Vector3f& wo) const;

    std::optional<bsdf_sample> sample_f(const Vector3f& wi, const Point2f& p) const;
};
