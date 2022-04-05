#include <Core/BxDF/Dielectric.hpp>

#include <Hinae/physics.hpp>

Dielectric::Dielectric(f32 eta)
    : BxDF(Type(Type::Transmission | Type::Reflection)), eta(eta) {}

Spectrum Dielectric::f(const Vector3f& wi, const Vector3f& wo) const
{
    return {0};
}

f32 Dielectric::pdf(const Vector3f& wi, const Vector3f& wo) const
{
    return 0;
}

std::tuple<Spectrum, Vector3f, f32>
Dielectric::sample_f(const Vector3f& wi, const Vector3f& n, const Point2f& p) const
{
    f32 cos_theta = dot(wi, n);
    f32 F = Fresnel::dielectric(cos_theta, 1.0f, eta);
    Vector3f wo;
    if(p[0] < F)
    {
        wo = reflect(-wi, n);
        return {Spectrum{F / std::abs(dot(wo, n))}, wo, F};
    }
    else
    {
        f32 ref_idx = (cos_theta > 0) ? 1.0f / eta : eta;
        std::optional<Vector3f> has_refract = refract(wi, n, ref_idx);

        if(!has_refract) return {Spectrum{1}, reflect(-wi, n), 1};

        wo = has_refract.value();
        return {Spectrum{(1 - F) / std::abs(dot(wo, n))}, wo, 1 - F};
    }
}
