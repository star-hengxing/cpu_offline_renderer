#pragma once

#include "monte_carlo_integrator.hpp"

struct ambient_occlusion_integrator : public monte_carlo_integrator
{
    using monte_carlo_integrator::monte_carlo_integrator;

    virtual Spectrum Li(const Ray3f& ray, const Scene& scene, Sampler& sampler) const override;
};
