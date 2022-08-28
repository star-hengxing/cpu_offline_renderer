#pragma once

#include "Texture.hpp"

struct constant_texture : public Texture
{
private:
    const Spectrum albedo;

public:
    constant_texture(const Spectrum& albedo) : albedo(albedo) {}

    virtual Spectrum evaluate(const hit_record& record) const override
    {
        return albedo;
    }
};
