#include <Core/BxDF/Lambertian.hpp>

#include <Hinae/coordinate_system.hpp>

#include <Sample.hpp>

Lambertian::Lambertian(const Spectrum& albedo)
    : BxDF(Type(Type::Diffuse | Type::Reflection)), albedo(albedo) {}

Spectrum Lambertian::f(const Vector3f& wi, const Vector3f& wo) const
{
    return albedo * INV_PI<f32>;
}

f32 Lambertian::pdf(const Vector3f& wi, const Vector3f& wo) const
{
    return Sample::cosine_hemisphere_pdf(wo);
}

std::tuple<Spectrum, Vector3f, f32>
Lambertian::sample_f(const Vector3f& wi, const Point2f& p) const
{
    auto wo = as<Vector3, f32>(Sample::cosine_hemisphere(p));
    // wo.z = std::copysign(wo.z, wi.z);
    return {f(wi, wo), wo, pdf(wi, wo)}; 
}
