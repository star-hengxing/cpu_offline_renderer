#include <Core/BxDF/Dielectric.hpp>

#include <Hinae/physics.hpp>

#include <Core/BxDF/microfacet_distribution/trowbridge_reitz.hpp>
#include <global.hpp>

Dielectric::Dielectric(const trowbridge_reitz& GGX, f32 eta_i, f32 eta_t)
    : BxDF(bxdf_type(bxdf_type::Transmission | bxdf_type::Reflection | bxdf_type::Specular))
    , GGX(GGX), eta_i(eta_i), eta_t(eta_t) {}

Spectrum Dielectric::f(const Vector3f& wi, const Vector3f& wo) const
{
    if(GGX.is_smooth()) return 0;
    const f32 cos_i = Local::cos_theta(wi), cos_o = Local::cos_theta(wo);
    const bool is_reflect = cos_i * cos_o > 0;
    
    const f32 eta = is_reflect ? 1 : ((cos_o > 0) ? eta_i / eta_t : eta_t / eta_i);
    Vector3f wh = wi + wo * eta;
    if(is_zero(cos_i) || is_zero(cos_o) || is_zero(wh))
        return 0;

    wh.normalize();
    if(wh.z < 0) wh.z = -wh.z;

    if(dot(wi, wh) * cos_i < 0 || dot(wh, wo) * cos_o < 0)
        return 0;

    const f32 D = GGX.D(wh);
    const f32 G = GGX.G(wi, wo);
    const f32 F = Fresnel::dielectric(dot(wi, wh), eta_i, eta_t);
    if(is_reflect)
    {
        return D * G * F / std::abs(4 * cos_i * cos_o);
    }
    else
    {
        const f32 denom = pow2((dot(wo, wh)) + dot(wi, wh) / eta) * cos_i * cos_o;
        return D * G * (1 - F) * std::abs(dot(wo, wh) * dot(wi, wh) / denom);
    }
}

f32 Dielectric::pdf(const Vector3f& wi, const Vector3f& wo) const
{
    if(GGX.is_smooth()) return 0;
    const f32 cos_i = Local::cos_theta(wi), cos_o = Local::cos_theta(wo);
    const bool is_reflect = cos_i * cos_o > 0;
    
    const f32 eta = is_reflect ? 1 : ((cos_o > 0) ? eta_i / eta_t : eta_t / eta_i);
    Vector3f wh = wi + wo * eta;
    if(is_zero(cos_i) || is_zero(cos_o) || is_zero(wh))
        return 0;

    wh.normalize();
    if(wh.z < 0) wh.z = -wh.z;

    if(dot(wi, wh) * cos_i < 0 || dot(wh, wo) * cos_o < 0)
        return 0;

    if(is_reflect)
    {
        return GGX.pdf(wi, wh) / (4 * dot(wi, wh));
    }
    else
    {
        const f32 denom = pow2((dot(wo, wh)) + dot(wi, wh) / eta);
        const f32 dwh_dwi = std::abs(dot(wo, wh)) / denom;
        return GGX.pdf(wi, wh) * dwh_dwi;
    }
}

std::optional<bxdf_sample> Dielectric::sample_f(const Vector3f& wi, const Point2f& p) const
{
    if(GGX.is_smooth())
    {
        const f32 cos = Local::cos_theta(wi);
        const f32 R = Fresnel::dielectric(cos, eta_i, eta_t);
        const f32 T = 1 - R;

        Vector3f wo;
        Spectrum f;
        f32 pdf;
        if(p[0] < R / (R + T))
        {
            wo = reflect(wi);
            f = R / Local::abs_cos_theta(wo);
            pdf = R / (R + T);
            return bxdf_sample{f, wo, pdf, bxdf_type::Reflection | bxdf_type::Specular};
        }
        else
        {
            const auto [ref_idx, normal] = (cos > 0)
                ? std::tuple{eta_i / eta_t, Vector3f{0, 0, 1}}
                : std::tuple{eta_t / eta_i, Vector3f{0, 0, -1}};

            const std::optional<Vector3f> is_refract = refract(-wi, normal, ref_idx);
            // total internal reflection
            if(!is_refract) return {};

            wo = *is_refract;
            f = T / Local::abs_cos_theta(wo);
            pdf = T / (R + T);
            return bxdf_sample{f, wo, pdf, bxdf_type::Transmission | bxdf_type::Specular};
        }
    }
    else
    {
        const Vector3f wh = GGX.sample_wm(wi, p);
        const f32 R = Fresnel::dielectric(dot(wi, wh), eta_i, eta_t);
        const f32 T = 1 - R;

        Vector3f wo;
        Spectrum f;
        f32 pdf;
        if(p[0] < R / (R + T))
        {
            wo = reflect(wi);
            if(!is_same_hemisphere(wi, wo)) return {};
            pdf = GGX.pdf(wi, wh) / (4 * dot(wi, wh)) * (R / (R + T));

            const f32 D = GGX.D(wh);
            const f32 G = GGX.G(wi, wo);
            f = D * G * R / (4 * Local::cos_theta(wi) * Local::cos_theta(wo));
            return bxdf_sample{f, wo, pdf, bxdf_type::Reflection | bxdf_type::Specular};
        }
        else
        {
            const auto ref_idx = (dot(wi, wh) > 0) ? (eta_i / eta_t) : (eta_t / eta_i);

            const std::optional<Vector3f> is_refract = refract(-wi, wh, ref_idx);
            // total internal reflection
            if(!is_refract) return {};

            wo = *is_refract;
            if(is_zero(wo.z) || is_same_hemisphere(wi, wo))
                return {};

            const f32 denom = pow2((dot(wo, wh)) + dot(wi, wh) / ref_idx);
            const f32 dwh_dwi = std::abs(dot(wo, wh)) / denom;

            pdf = GGX.pdf(wi, wh) * dwh_dwi * (T / (R + T));

            const f32 D = GGX.D(wh);
            const f32 G = GGX.G(wi, wo);
            f = D * G * T * std::abs(dot(wo, wh) * dot(wi, wh) / (Local::cos_theta(wi) * Local::cos_theta(wo) * denom));
            return bxdf_sample{f, wo, pdf, bxdf_type::Transmission | bxdf_type::Specular};
        }
    }
}
