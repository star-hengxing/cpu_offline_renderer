#pragma once

#include <math/basic_type.hpp>

#include <raytracing/Core/BxDF/microfacet_distribution/trowbridge_reitz.hpp>
#include "Material.hpp"

struct Glass : public Material
{
private:
    const trowbridge_reitz GGX;
    const f32 eta;
    
public:
    Glass(f32 alpha_x, f32 alpha_y, f32 eta);

    virtual void compute(hit_record& record) const;
};
