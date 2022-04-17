#pragma once

#include "BxDF.hpp"

struct Specular : public BxDF
{
private:
    Spectrum R;

public:
    Specular(const Spectrum& R);

    virtual Spectrum f(const Vector3f& wi, const Vector3f& wo) const override;

    virtual f32 pdf(const Vector3f& wi, const Vector3f& wo) const override;

    virtual std::optional<bxdf_sample> sample_f(const Vector3f& wi, const Point2f& p) const override;
};
