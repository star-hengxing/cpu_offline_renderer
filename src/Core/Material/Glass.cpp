#include <Core/Material/Glass.hpp>

#include <Core/BxDF/Dielectric.hpp>
#include <BxDF_memory.hpp>
#include <hit_record.hpp>

Glass::Glass(f32 eta) : eta(eta) {}

void Glass::compute(hit_record& record) const
{
    record.bxdf = BxDF_memory_pool::get().alloc<Dielectric>(eta);
}
