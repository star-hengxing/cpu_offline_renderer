#pragma once

#include "Material.hpp"

#include <memory>

#include <Core/Texture/Texture.hpp>

#include <Hinae/Vector3.hpp>

using Spectrum = Hinae::Vector3f;

struct Matte : public Material
{
private:
    const std::shared_ptr<Texture<Spectrum>> kd;        

public:
    Matte(std::shared_ptr<Texture<Spectrum>> kd);

    virtual void compute(hit_record& record) const;
};
