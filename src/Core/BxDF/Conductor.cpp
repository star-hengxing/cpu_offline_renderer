#include <Core/BxDF/Conductor.hpp>

#include <Hinae/physics.hpp>

#include <util/util.hpp>
#include <global.hpp>

Conductor::Conductor(const std::complex<f32>& eta_i,
    const std::array<f32, 3>& n,
    const std::array<f32, 3>& k,
    const trowbridge_reitz& GGX)
    : BxDF(bxdf_type(bxdf_type::Specular | bxdf_type::Reflection | bxdf_type::Glossy))
    , eta_i(eta_i), n(n), k(k), GGX(GGX) {}

static Spectrum fresnel(f32 cos, std::complex<f32> eta_i,
    const std::array<f32, 3>& n, const std::array<f32, 3>& k)
{
    Spectrum F;
    for(auto i : range(3))
    {
        const std::complex eta_t{n[i], k[i]};
        F[i] = Fresnel::conductor<false>(cos, eta_i, eta_t);
    }
    return F;
}

Spectrum Conductor::f(const Vector3f& wi, const Vector3f& wo) const
{
    if(GGX.is_smooth() || !is_same_hemisphere(wi, wo)) return 0;

    f32 cos_i = Local::abs_cos_theta(wi);
    f32 cos_o = Local::abs_cos_theta(wo);
    Vector3f wh = wi + wo;
    if(is_zero(cos_i) || is_zero(cos_o) || is_zero(wh))
        return 0;

    wh.normalize();

    const Spectrum F = fresnel(cos_i, eta_i, n, k);

    const f32 D = GGX.D(wh);
    const f32 G = GGX.G(wi, wo);
    
    return D * G * F / (4 * cos_i * cos_o);
}

f32 Conductor::pdf(const Vector3f& wi, const Vector3f& wo) const
{
    if(GGX.is_smooth() || !is_same_hemisphere(wi, wo)) return 0;

    Vector3f wh = wi + wo;
    if(is_zero(wh)) return 0;

    wh.normalize();
    return GGX.pdf(wi, wh) / (4 * std::abs(dot(wi ,wh)));
}

std::optional<bxdf_sample> Conductor::sample_f(const Vector3f& wi, const Point2f& p) const
{
    if(GGX.is_smooth())
    {
        const f32 pdf = 1;
        const Vector3f wo = reflect(wi);

        const f32 cos_i = Local::abs_cos_theta(wi);
        const f32 inv_cos_o = reciprocal(Local::abs_cos_theta(wo));

        const Spectrum f = fresnel(cos_i, eta_i, n, k) * inv_cos_o;
        return bxdf_sample{f, wo, pdf, bxdf_type::Specular};
    }
    else
    {
        if(is_zero(wi.z)) return {};

        const Vector3f wh = GGX.sample_wm(wi, p);
        const Vector3f wo = Hinae::reflect(-wi, wh);

        if(!is_same_hemisphere(wi, wo)) return {};

        const f32 pdf = GGX.pdf(wi, wh) / (4 * std::abs(dot(wi ,wh)));
        
        const f32 cos_i = Local::abs_cos_theta(wi);
        const f32 cos_o = Local::abs_cos_theta(wo);

        if(is_zero(cos_i) || is_zero(cos_o)) return {};

        const Spectrum F = fresnel(cos_i, eta_i, n, k);
        
        const f32 D = GGX.D(wh);
        const f32 G = GGX.G(wi, wo);

        const Spectrum f = D * G * F / (4 * cos_i * cos_o);
        return bxdf_sample{f, wo, pdf, bxdf_type::Glossy};
    }
}
