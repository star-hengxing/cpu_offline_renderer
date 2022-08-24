#include <raytracing/Core/Material/Matte.hpp>

#include <raytracing/Core/BxDF/Lambertian.hpp>
#include <raytracing/BxDF_memory.hpp>
#include <raytracing/hit_record.hpp>

Matte::Matte(std::shared_ptr<Texture<Spectrum>> kd) : kd(kd) {}

void Matte::compute(hit_record& record) const
{
    BxDF* bxdf = BxDF_memory_pool::get().alloc<Lambertian>(kd->evaluate(record));
    record.bsdf = BSDF{record, bxdf};
}
