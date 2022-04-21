#include <Core/BxDF/Dielectric.hpp>

#include <Hinae/physics.hpp>

Dielectric::Dielectric(f32 eta_i, f32 eta_t)
    : BxDF(bxdf_type(bxdf_type::Transmission | bxdf_type::Reflection | bxdf_type::Specular))
    , eta_i(eta_i), eta_t(eta_t) {}

Spectrum Dielectric::f(const Vector3f& wi, const Vector3f& wo) const
{
    return {0};
}

f32 Dielectric::pdf(const Vector3f& wi, const Vector3f& wo) const
{
    return 0;
}

std::optional<bxdf_sample> Dielectric::sample_f(const Vector3f& wi, const Point2f& p) const
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
    }
    else
    {
        const auto [ref_idx, normal] = (cos > 0)
            ? std::tuple{eta_i / eta_t, Vector3f{0, 0, 1}}
            : std::tuple{eta_t / eta_i, Vector3f{0, 0, -1}};

        const std::optional<Hinae::Vector3f> is_refract = refract(-wi, normal, ref_idx);
        // total internal reflection
        if(!is_refract) return {};

        wo = *is_refract;
        f = T / Local::abs_cos_theta(wo);
        pdf = T / (R + T);
    }
    return bxdf_sample{f, wo, pdf, type};
}
