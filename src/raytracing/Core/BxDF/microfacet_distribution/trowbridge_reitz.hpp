#pragma once

#include <math/Vector3.hpp>
#include <math/Point2.hpp>
using Spectrum = Vector3f;

struct trowbridge_reitz
{
private:
    const f32 alpha_x, alpha_y;
    const bool is_smooth_;

public:
    trowbridge_reitz(f32 alpha_x, f32 alpha_y);

    f32 D(const Vector3f& w) const;

    f32 D(const Vector3f& w, const Vector3f& wm) const;

    f32 G1(const Vector3f& w) const;

    f32 G(const Vector3f& wi, const Vector3f& wo) const;

    f32 lambda(const Vector3f& w) const;

    Vector3f sample_wm(const Vector3f& w, const Point2f& p) const;

    f32 pdf(const Vector3f& wi, const Vector3f& wh) const;
    
    bool is_smooth() const;
};
