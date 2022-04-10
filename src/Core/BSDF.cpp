#include <Core/BSDF.hpp>

#include <Core/BxDF/BxDF.hpp>
#include <hit_record.hpp>

#include <Hinae/coordinate_system.hpp>

Vector3f BSDF::to_local(const Vector3f& w) const
{
    return world_to_local(t, b, n, w);
}

Vector3f BSDF::to_world(const Vector3f& w) const
{
    return local_to_world(t, b, n, w);
}

BSDF::BSDF(const hit_record& record, BxDF* bxdf)
{
    this->bxdf = bxdf;
    this->n = record.n;
    const auto [t, b] = local_coordinate_system(n);
    this->t = t;
    this->b = b;
}

Spectrum BSDF::f(const Vector3f& wi, const Vector3f& wo) const
{
    return bxdf->f(to_local(wi), to_local(wo));
}

std::tuple<Spectrum, Vector3f, f32>
BSDF::sample_f(const Vector3f& wi, const Point2f& p) const
{
    const auto local_wi = to_local(wi);
    auto [f, wo, pdf] = bxdf->sample_f(local_wi, p);
    wo = to_world(wo);
    return {f, wo, pdf};
}
