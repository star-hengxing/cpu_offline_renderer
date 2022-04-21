#pragma once

#include "BxDF.hpp"

struct Dielectric : public BxDF
{
private:
    const f32 eta_i, eta_t;

public:
    Dielectric(f32 eta_i, f32 eta_t);

    virtual Spectrum f(const Vector3f& wi, const Vector3f& wo) const override;

    virtual f32 pdf(const Vector3f& wi, const Vector3f& wo) const override;

    virtual std::optional<bxdf_sample> sample_f(const Vector3f& wi, const Point2f& p) const override;
};
