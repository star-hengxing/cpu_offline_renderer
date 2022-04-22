#include <Core/Material/Glossy.hpp>

#include <Core/BxDF/Microfacet.hpp>
#include <BxDF_memory.hpp>
#include <hit_record.hpp>

static f32 roughness_to_alpha(f32 roughness)
{
    return std::sqrt(roughness);
}

Glossy::Glossy(const Spectrum& R, f32 alpha_x, f32 alpha_y, f32 eta)
    : R(R)
    , alpha_x(roughness_to_alpha(alpha_x))
    , alpha_y(roughness_to_alpha(alpha_y))
    , eta(eta) {}

void Glossy::compute(hit_record& record) const
{
    BxDF* bxdf = BxDF_memory_pool::get().alloc<Microfacet>(R, alpha_x, alpha_y, eta);
    record.bsdf = BSDF{record, bxdf};
}
