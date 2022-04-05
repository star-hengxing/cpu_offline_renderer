#include <Core/Material/Mirror.hpp>

#include <Core/BxDF/Specular.hpp>
#include <BxDF_memory.hpp>
#include <hit_record.hpp>

void Mirror::compute(hit_record& record) const
{
    record.bxdf = BxDF_memory_pool::get().alloc<Specular>(Spectrum{1});
}
