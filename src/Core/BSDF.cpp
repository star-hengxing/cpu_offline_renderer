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

std::optional<bsdf_sample> BSDF::sample_f(const Vector3f& wi, const Point2f& p) const
{
    std::optional<bxdf_sample> optional = bxdf->sample_f(to_local(wi), p);
    if(!optional) return {};

    auto [f, wo, pdf, type] = *optional;
    if(is_zero(f.norm2()) || is_zero(pdf)) return {};

    wo = to_world(wo);
    bool is_specular_ = BxDF::is_specular(type);
    return bsdf_sample{f, wo, pdf, is_specular_};
}
