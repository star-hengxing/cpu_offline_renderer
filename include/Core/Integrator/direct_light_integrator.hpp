#pragma once

#include "Integrator.hpp"

struct direct_light_integrator : public Integrator
{
    virtual Spectrum Li(const Ray3f& ray, const Scene& scene, Sampler& sampler) const override;
};
