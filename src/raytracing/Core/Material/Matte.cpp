#include <raytracing/Core/BxDF/Lambertian.hpp>
#include <raytracing/Core/Material/Matte.hpp>
#include <raytracing/BxDF_memory.hpp>
#include <raytracing/hit_record.hpp>

Matte::Matte(std::unique_ptr<Texture>&& texture) : texture(std::move(texture)) {}

void Matte::compute(hit_record& record) const
{
    BxDF* bxdf = BxDF_memory_pool::get().alloc<Lambertian>(texture->evaluate(record));
    record.bsdf = BSDF{record, bxdf};
}
