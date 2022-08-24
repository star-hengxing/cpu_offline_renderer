#include <math/Trigonometric.hpp>
#include <math/physics.hpp>

#include <raytracing/Core/BxDF/Microfacet.hpp>
#include <raytracing/global.hpp>

Microfacet::Microfacet(const Spectrum& R, f32 alpha_x, f32 alpha_y, f32 eta)
    : BxDF(bxdf_type(bxdf_type::Reflection | bxdf_type::Glossy))
    , R(R), distribution(alpha_x, alpha_y), eta(eta) {}

Spectrum Microfacet::f(const Vector3f& wi, const Vector3f& wo) const
{
    f32 cos_i = Local::abs_cos_theta(wi);
    f32 cos_o = Local::abs_cos_theta(wo);
    Vector3f wh = wi + wo;
    if(is_zero(cos_i) || is_zero(cos_o) || is_zero(wh))
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
    return distribution.pdf(wi, wh) / (4 * dot(wi, wh));
}

std::optional<bxdf_sample> Microfacet::sample_f(const Vector3f& wi, const Point2f& p) const
{
    Vector3f wh = distribution.sample_wm(wi, p);
    if(!is_same_hemisphere(wi, wh)) wh = -wh;
    Vector3f wo = ::reflect(-wi, wh);
    return bxdf_sample{f(wi, wo), wo, distribution.pdf(wi, wh) / (4 * dot(wi, wh)), type};
}
