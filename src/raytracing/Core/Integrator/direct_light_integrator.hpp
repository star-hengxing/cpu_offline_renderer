#pragma once

#include "Integrator.hpp"

struct hit_record;

struct direct_light_integrator : public Integrator
{
    static Spectrum sample(const Ray3f& ray, const Scene& scene, const hit_record& record, Sampler& sampler);

    virtual Spectrum Li(const Ray3f& ray, const Scene& scene, Sampler& sampler) const override;
};
