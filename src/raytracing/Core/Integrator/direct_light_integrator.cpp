#include <raytracing/Core/Integrator/direct_light_integrator.hpp>

#include <raytracing/Core/Sampler/Sampler.hpp>
#include <raytracing/Core/Light/Light.hpp>
#include <raytracing/Scene/Scene.hpp>
#include <raytracing/hit_record.hpp>
#include <raytracing/Core/BSDF.hpp>

Spectrum direct_light_integrator::sample(const Ray3f& ray, const Scene& scene, const hit_record& record, Sampler& sampler)
{
    Spectrum radiance{0};
    const Vector3f& n = record.n;
    const Point3f& p = record.p;
    const Vector3f wi = -ray.direction;
    for(const auto& light : scene.lights)
    {
        const std::optional<light_sample> payload = light->sample_li(p, sampler.get_2D());
        if(!payload) continue;

        const auto [wl, light_n, t, pdf, Li] = payload.value();

        const f32 cos = dot(n, wl);
        if(cos < 0) continue;

        const Ray3f shadow_ray = record.new_ray(wl);
        if(scene.bvh_intersect_p(shadow_ray, t - 0.001f)) continue;

        const Spectrum f = record.bsdf.f(wi, wl);
        radiance += Li * f * cos / pdf;
    }
    return radiance;
}

Spectrum direct_light_integrator::Li(const Ray3f& ray, const Scene& scene, Sampler& sampler) const
{
    const std::optional<hit_record> record = scene.bvh_intersect(ray);
    if(!record) return Spectrum{0};
    // return (record->n + 1) / 2;

    const Vector3f wi = -ray.direction;
    if(record->is_light())
    {
        return record->light->Li(record.value(), wi);
    }

    return sample(ray, scene, record.value(), sampler);
}
