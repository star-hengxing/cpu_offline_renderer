#include <Core/Material/Glass.hpp>

#include <Core/BxDF/Dielectric.hpp>
#include <BxDF_memory.hpp>
#include <hit_record.hpp>

Glass::Glass(f32 alpha_x, f32 alpha_y, f32 eta) : GGX(alpha_x, alpha_y), eta(eta) {}

void Glass::compute(hit_record& record) const
{
    BxDF* bxdf = BxDF_memory_pool::get().alloc<Dielectric>(GGX, 1.0f, eta);
    record.bsdf = BSDF{record, bxdf};
}
