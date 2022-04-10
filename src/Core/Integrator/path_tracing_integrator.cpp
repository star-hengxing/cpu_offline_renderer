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

    Ray3f trace_ray = ray;
    for([[maybe_unused]] usize bonus = 0;; bonus += 1)
    {
        const std::optional<hit_record> record = scene.bvh_intersect(trace_ray);
        if(!record) return radiance;

        const Vector3f n  = record->n;
        const Point3f  p  = record->p;
        const Vector3f wi = -trace_ray.direction;
        // direct radiance
        // if(!BxDF::is_specular(record->bsdf.type))
        // {
            for(const auto& light : scene.lights)
            {
                const auto [dir, t] = light->get_dir_and_distance(p);
                const Ray3f shadow_ray = record->new_ray(dir);

                const std::optional<hit_record> is_shadow = scene.bvh_intersect_p(shadow_ray);
                if(is_shadow && is_shadow->t_min <= t) continue;

                const Spectrum Li = light->Li(p);
                const Spectrum f = record->bsdf.f(wi, dir);
                const f32 cos = max(0.0f, dot(n, dir));

                const Spectrum direct = Li * f * cos;
                radiance += direct * tmp;
            }
        // }
        // indirect radiance
        const auto [f, wo, pdf] = record->bsdf.sample_f(wi, sampler.get_2D());
        const f32 cos = max(0.0f, dot(n, wo));
        tmp *= f * cos / pdf;
        trace_ray = record->new_ray(wo);
    }
    return radiance;
}
