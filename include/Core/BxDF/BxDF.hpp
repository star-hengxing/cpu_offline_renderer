#pragma once

#include <optional>

#include <Hinae/Vector3.hpp>
#include <Hinae/Point2.hpp>

using namespace Hinae;

using Spectrum = Vector3f;

enum class bxdf_type
{
    None         = 0b00000,
    Diffuse      = 0b00001,
    Reflection   = 0b00010,
    Specular     = 0b00100,
    Glossy       = 0b01000,
    Transmission = 0b10000,
    All          = Diffuse | Reflection | Specular | Glossy | Transmission,
};

inline bxdf_type operator | (bxdf_type type_1, bxdf_type type_2)
{
    return bxdf_type(static_cast<int>(type_1) | static_cast<int>(type_2));
}

struct bxdf_sample
{
    Spectrum f;
    Vector3f wo;
    f32 pdf;
    bxdf_type type;
};

struct BxDF
{
public:
    const bxdf_type type;

    static constexpr Vector3f NORMAL{0, 0, 1};

protected:
    Vector3f reflect(const Vector3f& w) const;

    bool is_same_hemisphere(const Vector3f& wi, const Vector3f& wo) const;

public:
    BxDF(bxdf_type type);

    virtual ~BxDF() = default;

    static bool is_specular(bxdf_type type);

    virtual Spectrum f(const Vector3f& wi, const Vector3f& wo) const = 0;
    
    virtual f32 pdf(const Vector3f& wi, const Vector3f& wo) const = 0;

    virtual std::optional<bxdf_sample> sample_f(const Vector3f& wi, const Point2f& p) const = 0;
};
