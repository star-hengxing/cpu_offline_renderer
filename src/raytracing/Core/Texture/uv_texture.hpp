#pragma once

#include <raytracing/hit_record.hpp>
#include "Texture.hpp"

struct uv_texture : public Texture
{
    virtual Spectrum evaluate(const hit_record& record) const override
    {
        const auto [u, v] = record.uv;
        return {u, v, 0};
    }
};
