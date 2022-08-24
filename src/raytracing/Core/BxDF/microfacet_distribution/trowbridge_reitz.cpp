#include <raytracing/Core/BxDF/microfacet_distribution/trowbridge_reitz.hpp>

#include <math/Trigonometric.hpp>

trowbridge_reitz::trowbridge_reitz(f32 alpha_x, f32 alpha_y)
    : alpha_x(alpha_x), alpha_y(alpha_y)
    , is_smooth_(max(alpha_x, alpha_y) <= 0.001f) {}

f32 trowbridge_reitz::G1(const Vector3f& w) const
{
    return 1 / (1 + lambda(w));
}

f32 trowbridge_reitz::G(const Vector3f& wi, const Vector3f& wo) const
{
    return 1 / (1 + lambda(wi) + lambda(wo));
}

f32 trowbridge_reitz::pdf(const Vector3f& wi, const Vector3f& wh) const
{
    return D(wi, wh);
}

f32 trowbridge_reitz::D(const Vector3f& w, const Vector3f& wm) const
{
    return G1(w) / Local::abs_cos_theta(w) * D(wm) * std::abs(dot(w, wm));
}

f32 trowbridge_reitz::D(const Vector3f& w) const
{
    if(alpha_x == alpha_y)
    {
        const f32 alpha2 = pow2(alpha_x);
        return alpha2 / (PI<f32> * pow2((alpha2 - 1) * Local::cos2_theta(w) + 1));
    }
    else
    {
        const f32 tan2 = Local::tan2_theta(w);
        if(std::isinf(tan2)) return 0;
        const f32 cos4 = pow4(Local::cos_theta(w));
        const f32 e = tan2 * (pow2(Local::cos2_phi(w) / alpha_x) + pow2(Local::sin2_phi(w) / alpha_y));
        return 1 / (PI<f32> * alpha_x * alpha_y * cos4 * pow2(1 + e));
    }
}

f32 trowbridge_reitz::lambda(const Vector3f& w) const
{
    const f32 tan2 = std::abs(Local::tan2_theta(w));
    if(std::isinf(tan2)) return 0;
    const f32 alpha2 = (alpha_x == alpha_y ? pow2(alpha_x) : pow2(Local::cos2_phi(w) * alpha_x) + pow2(Local::sin2_phi(w) * alpha_y));
    return (-1 + std::sqrt(1 + alpha2 * tan2)) / 2;
}

Vector3f trowbridge_reitz::sample_wm(const Vector3f& w, const Point2f& p) const
{
    Vector3f wh = Vector3f(alpha_x * w.x, alpha_y * w.y, w.z).normalized();
    if(wh.z < 0) wh = -wh;

    const Vector3f T1 = (wh.z < 0.99999f) ? cross(Vector3f(0, 0, 1), wh).normalized()
                                          : Vector3f(1, 0, 0);
    const Vector3f T2 = cross(wh, T1);

    const f32 a = 1.0f / (1.0f + wh.z);
    const f32 r = std::sqrt(p.x);
    const f32 phi = (p.y < a) ? p.y / a * PI<f32> : PI<f32> + (p.y - a) / (1.0f - a) * PI<f32>;

    Vector2f v
    {
        r * std::cos(phi),
        r * std::sin(phi) * ((p.y < a) ? 1.0f : wh.z)
    };

    // v.y = lerp(std::sqrt(1 - pow2(v.x)), v.y, (1 + wh.z) / 2);

    const f32 vx = std::sqrt(max(0.0f, 1 - v.norm2()));
    const Vector3f n = v.x * T1 + v.y * T2 + vx * wh;
    return Vector3f(alpha_x * n.x, alpha_y * n.y, max(0.0f, n.z)).normalized();
}

bool trowbridge_reitz::is_smooth() const
{
    return is_smooth_;
}
