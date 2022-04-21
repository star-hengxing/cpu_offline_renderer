#include <Core/Material/Glass.hpp>

#include <Core/BxDF/Dielectric.hpp>
#include <BxDF_memory.hpp>
#include <hit_record.hpp>

Glass::Glass(f32 eta) : eta(eta) {}

void Glass::compute(hit_record& record) const
{
    BxDF* bxdf = BxDF_memory_pool::get().alloc<Dielectric>(1.0f, eta);
    record.bsdf = BSDF{record, bxdf};
}
