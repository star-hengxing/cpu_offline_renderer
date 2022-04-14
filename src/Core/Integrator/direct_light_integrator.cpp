#include <Core/Integrator/direct_light_integrator.hpp>

#include <Core/Sampler/Sampler.hpp>
#include <Core/Light/Light.hpp>
#include <Core/BxDF/BxDF.hpp>
#include <Scene/Scene.hpp>
#include <hit_record.hpp>

Spectrum direct_light_integrator::Li(const Ray3f& ray, const Scene& scene, Sampler& sampler) const
{
    Spectrum color{0};
    std::optional<hit_record> record = scene.bvh_intersect(ray);
    if(!record) return color;

    const Vector3f wi = -ray.direction;
    if(record->is_light()) return record->light->Li(*record, wi);

    for(const auto& light : scene.lights)
    {
        const auto [light_dir, t, pdf] = light->sample_li(record->p, sampler.get_2D());
        const Ray3f shadow_ray = record->new_ray(light_dir);

        const std::optional<hit_record> is_shadow = scene.bvh_intersect_p(shadow_ray, t);
        if(is_shadow) continue;

        const Spectrum Li = light->Li(*record, -light_dir);
        const Spectrum f = record->bsdf.f(wi, light_dir);
        const f32 cos = max(0.0f, dot(record->n, light_dir));

        color += Li * f * cos;
    }
    return color;
    // return (record->n + 1) / 2;
}
