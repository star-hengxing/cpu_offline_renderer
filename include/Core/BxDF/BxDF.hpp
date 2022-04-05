#pragma once

#include <tuple>

#include <Hinae/Vector3.hpp>
#include <Hinae/Point2.hpp>

using namespace Hinae;

using Spectrum = Vector3f;

struct BxDF
{
public:
    enum Type : usize
    {
        Diffuse      = 0b00001,
        Reflection   = 0b00010,
        Specular     = 0b00100,
        Glossy       = 0b01000,
        Transmission = 0b10000,
        All          = Diffuse | Reflection | Specular | Glossy | Transmission,
    };

    const Type type;

public:
    BxDF(Type type);

    virtual ~BxDF() = default;

    static bool is_specular(Type type);

    virtual Spectrum f(const Vector3f& wi, const Vector3f& wo) const = 0;
    
    virtual f32 pdf(const Vector3f& wi, const Vector3f& wo) const = 0;
    // radiance, sample direction, pdf
    virtual std::tuple<Spectrum, Vector3f, f32>
    sample_f(const Vector3f& wi, const Vector3f& n, const Point2f& p) const = 0;
};
