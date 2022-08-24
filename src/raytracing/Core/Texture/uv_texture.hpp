#pragma once

#include <math/Vector3.hpp>

#include <raytracing/hit_record.hpp>
#include "Texture.hpp"

using Spectrum = Vector3f;

struct uv_texture : public Texture<Spectrum>
{
public:
    uv_texture() = default;

    virtual Spectrum evaluate(const hit_record& record) const
    {
        const auto [u, v] = record.uv;
        return {u, v, 0};
    }
};
