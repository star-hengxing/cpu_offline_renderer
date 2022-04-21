#pragma once

#include <array>

#include "Material.hpp"

#include <Hinae/basic_type.hpp>

using namespace Hinae;

struct Metal : public Material
{
private:
    const std::array<f32, 3> n;
    const std::array<f32, 3> k;
    
public:
    Metal(const std::array<f32, 3>& n, const std::array<f32, 3>& k);

    virtual void compute(hit_record& record) const;
};
