#pragma once

#include "Integrator.hpp"

struct path_tracing_integrator : public Integrator
{
    virtual Spectrum Li(const Ray3f& ray, const Scene& scene, Sampler& sampler) const override;
};
