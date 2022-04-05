#pragma once

#include "Material.hpp"

#include <Hinae/basic_type.hpp>

using namespace Hinae;

struct Glass : public Material
{
private:
    const f32 eta;
    
public:
    Glass(f32 eta);

    virtual void compute(hit_record& record) const;
};
