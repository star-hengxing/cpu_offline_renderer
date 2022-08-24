#pragma once

#include <memory>

#include <math/Vector3.hpp>

#include <raytracing/Core/Texture/Texture.hpp>
#include "Material.hpp"

using Spectrum = Vector3f;

struct Matte : public Material
{
private:
    const std::shared_ptr<Texture<Spectrum>> kd;        

public:
    Matte(std::shared_ptr<Texture<Spectrum>> kd);

    virtual void compute(hit_record& record) const;
};
