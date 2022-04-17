#include <Core/Integrator/path_tracing_integrator.hpp>

#include <Core/Sampler/Sampler.hpp>
#include <Core/Light/Light.hpp>
#include <Core/BxDF/BxDF.hpp>
#include <Scene/Scene.hpp>
#include <hit_record.hpp>

Spectrum path_tracing_integrator::Li(const Ray3f& ray, const Scene& scene, Sampler& sampler) const
{
    Spectrum radiance{0};
    Spectrum tmp{1};
    bool specular_bounce = false;

    Ray3f trace_ray = ray;
    for(usize bounce = 0;; bounce += 1)
    {
        const std::optional<hit_record> record = scene.bvh_intersect(trace_ray);
        if(!record) return radiance;

        const Vector3f n  = record->n;
        const Point3f  p  = record->p;
        const Vector3f wi = -trace_ray.direction;
        
        if(record->is_light())
        {
            if(bounce == 0 || specular_bounce)
            {
                radiance += record->light->Li(*record, wi) * tmp;
            }
            return radiance;
        }
        // direct radiance
        for(const auto& light : scene.lights)
        {
            const std::optional<light_sample> payload = light->sample_li(p, sampler.get_2D());
            if(!payload) continue;

            const auto [wl, light_n, t, pdf, Li] = *payload;

            const Ray3f shadow_ray = record->new_ray(wl);
            if(scene.bvh_intersect_p(shadow_ray, t - 0.001f)) continue;

            const Spectrum f = record->bsdf.f(wi, wl);
            const f32 cos = max(0.0f, dot(n, wl));

            const Spectrum direct = Li * f * cos / pdf;
            radiance += direct * tmp;
        }
        // indirect radiance
        std::optional<bsdf_sample> bsdf = record->bsdf.sample_f(wi, sampler.get_2D());
        if(!bsdf) return radiance;

        const auto [f, wo, pdf, is_specular] = *bsdf;
        const f32 cos = std::abs(dot(n, wo));
        tmp *= f * cos / pdf;

        trace_ray = record->new_ray(wo);
        specular_bounce = is_specular;
    }
    return radiance;
}
