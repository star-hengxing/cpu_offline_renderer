#pragma once

#include <Hinae/basic_type.hpp>
#include <Hinae/Vector3.hpp>

#include "Material.hpp"

using namespace Hinae;
using Spectrum = Vector3f;

struct Glossy : public Material
{
private:
    const Spectrum R;
    const f32 alpha_x, alpha_y;
    const f32 eta;
    
public:
    Glossy(const Spectrum& R, f32 alpha_x, f32 alpha_y, f32 eta);

    virtual void compute(hit_record& record) const;
};
