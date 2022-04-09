#include <Core/BxDF/microfacet_distribution/Beckmann.hpp>

#include <Hinae/Trigonometric.hpp>

f32 Beckmann::D(const Vector3f& wh) const
{
    f32 tan2 = Local::tan2_theta(wh);
    if(std::isinf(tan2)) return 0;
    f32 cos2 = Local::cos2_theta(wh);
    f32 cos4 = cos2 * cos2;
    return std::exp(-tan2 * (Local::cos2_phi(wh) / (alpha_x * alpha_y)
                            + Local::sin2_phi(wh) / (alpha_x * alpha_y)))
                            / (PI<f32> * alpha_x * alpha_y * cos4);
}

f32 Beckmann::lambda(const Vector3f& w) const
{
    f32 tan = std::abs(Local::tan_theta(w));
    if(std::isinf(tan)) return 0;
    f32 alpha = std::sqrt(Local::cos2_phi(w) * alpha_x * alpha_x
                        + Local::sin2_phi(w) * alpha_y * alpha_y);
    f32 a = 1 / (alpha * tan);
    if(a >= 1.6f) return 0;
    return (1 - 1.259f * a + 0.396f * a * a) /
           (3.535f * a + 2.181f * a * a);
}
