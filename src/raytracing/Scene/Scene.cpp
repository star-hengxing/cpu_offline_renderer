#include <raytracing/Core/Primitive/geometry_primitive.hpp>
#include <raytracing/Scene/Scene.hpp>
#include <raytracing/hit_record.hpp>

#include <util/range.hpp>

Scene& Scene::build()
{
    bvh.build(primitives);
    return *this;
}

std::optional<hit_record> Scene::intersect(const Ray3f& ray) const
{
    hit_record record;
    isize      index = without_intersect;

    for (const auto& [i, v] : enumerate(primitives))
    {
        if (v->intersect(ray, record))
            index = i;
    }

    if (index == without_intersect) return {};
    primitives[index]->get_intersect_record(ray, record);
    primitives[index]->compute_BxDF(record);
    return record;
}

bool Scene::intersect_p(const Ray3f& shadow_ray, f32 t_max) const
{
    hit_record record;
    record.t_max = t_max;
    for (const auto& primitive : primitives)
    {
        if (primitive->intersect(shadow_ray, record))
            return true;
    }
    return false;
}

std::optional<hit_record> Scene::bvh_intersect(const Ray3f& ray) const
{
    return bvh.intersect(ray);
}

std::optional<hit_record> Scene::bvh_intersect_p(const Ray3f& shadow_ray, f32 t_max) const
{
    return bvh.intersect_p(shadow_ray, t_max);
}
