#include <Core/BxDF/Conductor.hpp>

#include <Hinae/physics.hpp>

#include <util/util.hpp>

Conductor::Conductor(const std::complex<f32>& eta_i, const std::array<f32, 3>& n, const std::array<f32, 3>& k)
    : BxDF(bxdf_type(bxdf_type::Specular | bxdf_type::Reflection))
    , eta_i(eta_i), n(n), k(k) {}

Spectrum Conductor::f(const Vector3f& wi, const Vector3f& wo) const
{
    return 0;
}

f32 Conductor::pdf(const Vector3f& wi, const Vector3f& wo) const
{
    return 0;
}

std::optional<bxdf_sample> Conductor::sample_f(const Vector3f& wi, const Point2f& p) const
{
    const f32 pdf = 1;
    const Vector3f wo = reflect(wi);
    const f32 cos_i = Local::cos_theta(wi);

    Spectrum R;
    const f32 cos_o = reciprocal(Local::abs_cos_theta(wo));
    for(auto i : range(3))
    {
        const std::complex eta_t{n[i], k[i]};
        R[i] = Fresnel::conductor(cos_i, eta_i, eta_t) * cos_o;
    }
    return bxdf_sample{R, wo, pdf, type};
}
