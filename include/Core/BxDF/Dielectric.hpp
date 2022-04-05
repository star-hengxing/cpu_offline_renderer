#pragma once

#include "BxDF.hpp"

struct Dielectric : public BxDF
{
private:
    const f32 eta;

public:
    Dielectric(f32 eta);

    virtual Spectrum f(const Vector3f& wi, const Vector3f& wo) const override;

    virtual f32 pdf(const Vector3f& wi, const Vector3f& wo) const override;

    virtual std::tuple<Spectrum, Vector3f, f32>
    sample_f(const Vector3f& wi, const Vector3f& n, const Point2f& p) const override;
};
