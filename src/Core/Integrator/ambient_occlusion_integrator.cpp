#include <Core/Integrator/ambient_occlusion_integrator.hpp>

#include <Core/Sampler/Sampler.hpp>
#include <Core/Light/Light.hpp>
#include <Scene/Scene.hpp>
#include <hit_record.hpp>
#include <util/util.hpp>
#include <Sample.hpp>
#include <global.hpp>

#include <Hinae/coordinate_system.hpp>

Spectrum ambient_occlusion_integrator::Li(const Ray3f& ray, const Scene& scene, Sampler& sampler) const
{
    Spectrum color = 0;
    std::optional<hit_record> record = scene.bvh_intersect(ray);
    if(!record) return color;

    Ray3f shadow_ray;
    const Vector3f& n = record->n;
    shadow_ray.origin = Offset::robust(record->p, n);
    for([[maybe_unused]] auto _ : range(spp))
    {
        // cosine-weighted importance sampling
        auto wo = as<Vector3, f32>(Sample::cosine_hemisphere(sampler.get_2D()));
        wo = local_to_world(n, wo);

        // auto wo = as<Vector3, f32>(Sample::uniform_hemisphere(next2D()));
        // const f32 pdf = Sample::uniform_hemisphere_pdf(wo);
        // const auto [t, b] = local_coordinate_system(n);
        // wo = local_to_world(t, b, n, wo);

        shadow_ray.direction = wo;
        if(!scene.bvh_intersect_p(shadow_ray))
            color += 1;

        // // without direct light, visibility is 0
        // if(!scene.bvh_intersect_p(shadow_ray))
        //     color += max(0.0f, dot(n, wo)) / pdf;
    }
    return color / static_cast<f32>(spp);
}
