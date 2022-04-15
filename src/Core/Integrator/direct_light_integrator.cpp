#include <Core/Integrator/direct_light_integrator.hpp>

#include <Core/Sampler/Sampler.hpp>
#include <Core/Light/Light.hpp>
#include <Core/BxDF/BxDF.hpp>
#include <Scene/Scene.hpp>
#include <hit_record.hpp>

Spectrum direct_light_integrator::Li(const Ray3f& ray, const Scene& scene, Sampler& sampler) const
{
    Spectrum color{0};
    const std::optional<hit_record> record = scene.bvh_intersect(ray);
    if(!record) return color;

    const Vector3f wi = -ray.direction;
    if(record->is_light()) return record->light->Li(*record, wi);

    for(const auto& light : scene.lights)
    {
        const std::optional<light_sample> payload = light->sample_li(record->p, sampler.get_2D());
        if(!payload) continue;

        const auto [wl, light_n, t, pdf, Li] = *payload;

        const Ray3f shadow_ray = record->new_ray(wl);
        if(scene.bvh_intersect_p(shadow_ray, t - 0.001f)) continue;

        const Spectrum f = record->bsdf.f(wi, wl);
        const f32 cos = max(0.0f, dot(record->n, wl));

        color += Li * f * cos / pdf;
    }
    return color;
    // return (record->n + 1) / 2;
}
