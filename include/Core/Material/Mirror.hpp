#pragma once

#include "Material.hpp"

struct Mirror : public Material
{
public:
    virtual void compute(hit_record& record) const;
};
