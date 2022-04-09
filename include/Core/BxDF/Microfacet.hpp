#pragma once

#include "BxDF.hpp"
#include "microfacet_distribution/trowbridge_reitz.hpp"

struct Microfacet : public BxDF
{
private:
    const Spectrum R;
    const trowbridge_reitz distribution;

public:
    Microfacet(const Spectrum& R, f32 alpha_x, f32 alpha_y);

    virtual Spectrum f(const Vector3f& wi, const Vector3f& wo) const override;

    virtual f32 pdf(const Vector3f& wi, const Vector3f& wo) const override;

    virtual std::tuple<Spectrum, Vector3f, f32>
    sample_f(const Vector3f& wi, const Point2f& p) const override;
};