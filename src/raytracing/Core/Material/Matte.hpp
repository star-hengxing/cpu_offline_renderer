#pragma once

#include <memory>

#include <raytracing/Core/Texture/Texture.hpp>
#include "Material.hpp"

struct Matte : public Material
{
private:
    const std::unique_ptr<Texture> texture;

public:
    Matte(std::unique_ptr<Texture>&& texture);

    virtual void compute(hit_record& record) const override;
};
