#include <Core/BxDF/microfacet_distribution/trowbridge_reitz.hpp>

#include <Hinae/Trigonometric.hpp>

f32 trowbridge_reitz::D(const Vector3f& wh) const
{
    f32 tan2 = Local::tan2_theta(wh);
    if(std::isinf(tan2)) return 0;
    f32 cos2 = Local::cos2_theta(wh);
    f32 cos4 = cos2 * cos2;
    f32 e = (Local::cos2_phi(wh) / (alpha_x * alpha_y) +
             Local::sin2_phi(wh) / (alpha_x * alpha_y)) * tan2;
    return 1 / (PI<f32> * alpha_x * alpha_y * cos4 * (1 + e) * (1 + e));
}

f32 trowbridge_reitz::lambda(const Vector3f& w) const
{
    f32 tan = std::abs(Local::tan_theta(w));
    if(std::isinf(tan)) return 0;
    f32 alpha = std::sqrt(Local::cos2_phi(w) * alpha_x * alpha_x
                        + Local::sin2_phi(w) * alpha_y * alpha_y);
    f32 alpha2 = (alpha * tan) * (alpha * tan);
    return (-1 + std::sqrt(1.0f + alpha2)) / 2;
}

std::tuple<Spectrum, Vector3f, f32>
trowbridge_reitz::sample_f(const Vector3f& wi, const Point2f& p) const
{
    
}
