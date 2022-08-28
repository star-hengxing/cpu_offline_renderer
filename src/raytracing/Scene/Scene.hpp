#pragma once

#include <optional>

#include <raytracing/Core/BVH/BVH.hpp>
#include <math/Matrix4.hpp>
#include <util/View.hpp>

struct geometry_primitive;
struct Light;

struct Scene
{
private:
    BVH bvh;

public:
    View<geometry_primitive*> primitives;
    View<Light*>              lights;

public:
    Scene() {}

    Scene& build();

    std::optional<hit_record> intersect(const Ray3f& ray) const;

    bool intersect_p(const Ray3f& shadow_ray, f32 t_max = INFINITY_<f32>) const;

    std::optional<hit_record> bvh_intersect(const Ray3f& ray) const;

    std::optional<hit_record> bvh_intersect_p(const Ray3f& shadow_ray, f32 t_max = INFINITY_<f32>) const;
};
