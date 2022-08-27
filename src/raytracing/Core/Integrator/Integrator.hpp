#pragma once

#include <math/Ray3.hpp>

using Spectrum = Vector3f;

struct Scene;
struct Sampler;

struct Integrator
{
    virtual Spectrum Li(const Ray3f& ray, const Scene& scene, Sampler& sampler) const = 0;
};
