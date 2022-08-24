#include <raytracing/Core/Integrator/path_tracing_integrator.hpp>
#include <raytracing/Core/Integrator/direct_light_integrator.hpp>

#include <raytracing/Core/Sampler/Sampler.hpp>
#include <raytracing/Core/Light/Light.hpp>
#include <raytracing/Scene/Scene.hpp>
#include <raytracing/hit_record.hpp>
#include <raytracing/Core/BSDF.hpp>

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
        const Spectrum direct = direct_light_integrator::sample(ray, scene, record.value(), sampler);
        radiance += direct * tmp;
        // indirect radiance
        std::optional<bsdf_sample> bsdf = record->bsdf.sample_f(wi, sampler.get_2D());
        if(!bsdf) return radiance;

        const auto [f, wo, pdf, is_specular] = *bsdf;
        const f32 cos = std::abs(dot(n, wo));
        tmp *= f * cos / pdf;

        trace_ray = record->new_ray(wo);
        specular_bounce = is_specular;
    }
}
