#include <Core/BxDF/Microfacet.hpp>

#include <Hinae/Trigonometric.hpp>
#include <Hinae/physics.hpp>

Microfacet::Microfacet(const Spectrum& R, f32 alpha_x, f32 alpha_y)
    : BxDF(Type(Type::Reflection | Type::Glossy))
    , R(R), distribution(alpha_x, alpha_y) {}

Spectrum Microfacet::f(const Vector3f& wi, const Vector3f& wo) const
{
    f32 cos_i = Local::abs_cos_theta(wi);
    f32 cos_o = Local::abs_cos_theta(wo);
    Vector3f wh = wi + wo;
    if(is_zero(cos_i) || is_zero(cos_o) || wh == Vector3f{0})
        return {0};

    wh.normalize();
    return R * distribution.D(wh) * distribution.G(wi, wo)
            / (4 * cos_i * cos_o);
}

f32 Microfacet::pdf(const Vector3f& wi, const Vector3f& wo) const
{
    
}

std::tuple<Spectrum, Vector3f, f32>
Microfacet::sample_f(const Vector3f& wi, const Point2f& p) const
{

}
