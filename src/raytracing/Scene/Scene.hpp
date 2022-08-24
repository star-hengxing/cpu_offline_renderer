#pragma once

#include <string_view>
#include <optional>
#include <vector>
#include <memory>

#include <raytracing/Core/BVH/BVH.hpp>

#include <math/Matrix4.hpp>

struct geometry_primitive;
struct Material;
struct Light;

using primitive_vector_type = std::vector<std::shared_ptr<geometry_primitive>>;
using light_vector_type = std::vector<std::shared_ptr<Light>>;

struct transform_vector
{
private:
    static constexpr Matrix4f identity_matrix = Matrix4f::identity();
    std::vector<std::unique_ptr<std::tuple<Matrix4f, Matrix4f>>> matrix;

public:
    transform_vector() = default;

    static const Matrix4f& identity();

    std::tuple<const Matrix4f&, const Matrix4f&> add(const Matrix4f& transform, const Matrix4f& inv);
};

struct Scene
{
private:
    BVH bvh;

public:
    primitive_vector_type primitives;
    light_vector_type lights;
    transform_vector transforms;

public:
    Scene() = default;

    Scene(primitive_vector_type&& primitives, light_vector_type&& lights);

    Scene& add(std::shared_ptr<Light> light);

    const Matrix4f& identity() const;

    std::tuple<const Matrix4f&, const Matrix4f&>
    add(const Matrix4f& transform);

    std::tuple<const Matrix4f&, const Matrix4f&>
    add(const Matrix4f& transform, const Matrix4f& inv);

    Scene& add(std::shared_ptr<geometry_primitive> primitive);

    Scene& add(
        const std::string_view& path,
        const Matrix4f& transform,
        std::shared_ptr<Material> material);

    Scene& build();

    std::optional<hit_record> intersect(const Ray3f& ray) const;

    bool intersect_p(const Ray3f& shadow_ray, f32 t_max = INFINITY_<f32>) const;

    std::optional<hit_record> bvh_intersect(const Ray3f& ray) const;

    std::optional<hit_record> bvh_intersect_p(const Ray3f& shadow_ray, f32 t_max = INFINITY_<f32>) const;
};
