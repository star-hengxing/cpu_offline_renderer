#pragma once

#include "BxDF.hpp"

struct Lambertian : public BxDF
{
private:
    Spectrum albedo;

public:
    Lambertian(const Spectrum& albedo);

    virtual Spectrum f(const Vector3f& wi, const Vector3f& wo) const override;

    virtual f32 pdf(const Vector3f& wi, const Vector3f& wo) const override;

    virtual std::optional<bxdf_sample> sample_f(const Vector3f& wi, const Point2f& p) const override;
};
