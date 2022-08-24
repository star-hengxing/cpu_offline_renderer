#include <raytracing/Core/Material/Metal.hpp>

#include <raytracing/Core/BxDF/Conductor.hpp>
#include <raytracing/BxDF_memory.hpp>
#include <raytracing/hit_record.hpp>

Metal::Metal(f32 alpha_x, f32 alpha_y, const std::array<f32, 3>& n, const std::array<f32, 3>& k)
    : GGX(alpha_x, alpha_y), n(n), k(k) {}

void Metal::compute(hit_record& record) const
{
    BxDF* bxdf = BxDF_memory_pool::get().alloc<Conductor>(std::complex<f32>{1, 0}, n, k, GGX);
    record.bsdf = BSDF{record, bxdf};
}
