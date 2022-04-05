#pragma once

#include <Hinae/Vector3.hpp>
#include <Hinae/Point2.hpp>

using namespace Hinae;

struct Microface
{
    virtual ~Microface() = default;

    static f32 roughness_to_alpha(f32 roughness);

    virtual f32 D(const Vector3f& wh) const = 0;

    virtual f32 lambda(const Vector3f& w) const = 0;

    f32 G1(const Vector3f& w) const;

    f32 G(const Vector3f& wi, const Vector3f& wo) const;

    virtual Vector3f Sample_f(
        const Vector3f& wi, const Vector3f& n, const Point2f& u) const = 0;

    virtual f32 PDF(const Vector3f& wi, const Vector3f& wh) const = 0;
};
