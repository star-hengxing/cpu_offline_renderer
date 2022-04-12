#include <Core/BxDF/Microfacet.hpp>

#include <Hinae/Trigonometric.hpp>
#include <Hinae/physics.hpp>
#include <global.hpp>

Microfacet::Microfacet(const Spectrum& R, f32 alpha_x, f32 alpha_y, f32 eta)
    : BxDF(Type(Type::Reflection | Type::Glossy))
    , R(R), distribution(alpha_x, alpha_y), eta(eta) {}

Spectrum Microfacet::f(const Vector3f& wi, const Vector3f& wo) const
{
    f32 cos_i = Local::abs_cos_theta(wi);
    f32 cos_o = Local::abs_cos_theta(wo);
    Vector3f wh = wi + wo;
    if(is_zero(cos_i) || is_zero(cos_o) || is_zero(wh.norm2()))
        return {0};

    wh.normalize();
    if(wh.z < 0) wh = -wh;
    f32 D = distribution.D(wh);
    f32 G = distribution.G(wi, wo);
    f32 F = Fresnel::dielectric(dot(wi, wh), 1.0f, eta);
    return R * D * G * F / (4 * cos_i * cos_o);
}

f32 Microfacet::pdf(const Vector3f& wi, const Vector3f& wo) const
{
    Vector3f wh = (wi + wo).normalized();
    return distribution.PDF(wi, wh) / (4 * dot(wi, wh));
}

std::tuple<Spectrum, Vector3f, f32>
Microfacet::sample_f(const Vector3f& wi, const Point2f& p) const
{
    Vector3f wh = distribution.sample_wm(wi, p);
    if(wi.z * wh.z < 0) wh = -wh;
    Vector3f wo = -reflect(wi, wh);
    return {f(wi, wo), wo, distribution.PDF(wi, wh) / (4 * dot(wi, wh))};
}
