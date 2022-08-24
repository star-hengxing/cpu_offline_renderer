#pragma once

#include <array>

#include <math/basic_type.hpp>

#include <raytracing/Core/BxDF/microfacet_distribution/trowbridge_reitz.hpp>
#include "Material.hpp"

struct Metal : public Material
{
private:
    const trowbridge_reitz GGX;
    const std::array<f32, 3> n;
    const std::array<f32, 3> k;
    
public:
    Metal(f32 alpha_x, f32 alpha_y, const std::array<f32, 3>& n, const std::array<f32, 3>& k);

    virtual void compute(hit_record& record) const;
};
