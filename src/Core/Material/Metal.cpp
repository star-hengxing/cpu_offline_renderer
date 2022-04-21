#include <Core/Material/Metal.hpp>

#include <Core/BxDF/Conductor.hpp>
#include <BxDF_memory.hpp>
#include <hit_record.hpp>

Metal::Metal(const std::array<f32, 3>& n, const std::array<f32, 3>& k)
    : n(n), k(k) {}

void Metal::compute(hit_record& record) const
{
    BxDF* bxdf = BxDF_memory_pool::get().alloc<Conductor>(std::complex<f32>{1, 0}, n, k);
    record.bsdf = BSDF{record, bxdf};
}
