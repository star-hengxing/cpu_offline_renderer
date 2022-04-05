#pragma once

#include <Hinae/Ray3.hpp>

using Ray3f    = Hinae::Ray3f;
using Spectrum = Hinae::Vector3f;

struct Scene;
struct Sampler;

struct Integrator
{
    virtual ~Integrator() = default;

    virtual Spectrum Li(const Ray3f& ray, const Scene& scene, Sampler& sampler) const = 0;
};
