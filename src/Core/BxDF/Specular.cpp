#include <Core/BxDF/Specular.hpp>

#include <Hinae/physics.hpp>

Specular::Specular(const Spectrum& R)
    : BxDF(bxdf_type(bxdf_type::Specular | bxdf_type::Reflection)), R(R) {}

Spectrum Specular::f(const Vector3f& wi, const Vector3f& wo) const
{
    return {0};
}

f32 Specular::pdf(const Vector3f& wi, const Vector3f& wo) const
{
    return 0;
}

std::optional<bxdf_sample> Specular::sample_f(const Vector3f& wi, const Point2f& p) const
{
    const Vector3f wo = reflect(wi);
    return bxdf_sample{Spectrum{1}, wo, 1, type};
}
