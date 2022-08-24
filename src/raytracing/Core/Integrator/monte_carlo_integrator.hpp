#pragma once

#include "Integrator.hpp"

struct monte_carlo_integrator : public Integrator
{
protected:
    const usize spp;

public:
    monte_carlo_integrator(usize spp) : spp(spp) {}

    virtual Spectrum Li(const Ray3f& ray, const Scene& scene, Sampler& sampler) const = 0;
};
