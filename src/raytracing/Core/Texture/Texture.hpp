#pragma once

#include <math/Vector3.hpp>

using Spectrum = Vector3f;

struct hit_record;

struct Texture
{
    virtual ~Texture() {}

    virtual Spectrum evaluate(const hit_record& record) const = 0;
};
