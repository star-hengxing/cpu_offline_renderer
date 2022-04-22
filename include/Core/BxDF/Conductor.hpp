#pragma once

#include <complex>
#include <array>

#include <Core/BxDF/microfacet_distribution/trowbridge_reitz.hpp>
#include "BxDF.hpp"

struct Conductor : public BxDF
{
private:
    const std::complex<f32> eta_i;
    const std::array<f32, 3>& n;
    const std::array<f32, 3>& k;
    const trowbridge_reitz GGX;

public:
    Conductor(const std::complex<f32>& eta_i,
        const std::array<f32, 3>& n,
        const std::array<f32, 3>& k,
        const trowbridge_reitz& GGX);

    virtual Spectrum f(const Vector3f& wi, const Vector3f& wo) const override;

    virtual f32 pdf(const Vector3f& wi, const Vector3f& wo) const override;

    virtual std::optional<bxdf_sample> sample_f(const Vector3f& wi, const Point2f& p) const override;
};
