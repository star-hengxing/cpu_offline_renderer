#include <Core/Material/Matte.hpp>

#include <Core/BxDF/Lambertian.hpp>
#include <BxDF_memory.hpp>
#include <hit_record.hpp>

Matte::Matte(std::shared_ptr<Texture<Spectrum>> kd) : kd(kd) {}

void Matte::compute(hit_record& record) const
{
    BxDF* bxdf = BxDF_memory_pool::get().alloc<Lambertian>(kd->evaluate(record));
    record.bsdf = BSDF{record, bxdf};
}
