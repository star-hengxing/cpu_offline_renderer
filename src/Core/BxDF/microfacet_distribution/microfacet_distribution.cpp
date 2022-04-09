#include <Core/BxDF/microfacet_distribution/microfacet_distribution.hpp>

f32 microfacet_distribution::roughness_to_alpha(f32 roughness)
{
    roughness = max(roughness, 0.001f);
    f32 x = std::log(roughness);
    f32 x2 = x * x;
    f32 x3 = x2 * x;
    f32 x4 = x3 * x;
    return 1.62142f + 0.819955f * x + 0.1734f * x2 + 0.0171201f * x3 + 0.000640711f * x4;
}

f32 microfacet_distribution::G1(const Vector3f& w) const
{
    return 1 / (1 + lambda(w));
}

f32 microfacet_distribution::G(const Vector3f& wi, const Vector3f& wo) const
{
    return 1 / (1 + lambda(wo) + lambda(wi));
}
