#pragma once

#include "Primitive.hpp"

#include <memory>

struct Shape;
struct Light;
struct Material;

struct geometry_primitive : public Primitive
{
private:
    std::shared_ptr<Shape> shape;
    std::shared_ptr<Material> material;
    std::shared_ptr<Light> light;

public:
    geometry_primitive() = default;

    geometry_primitive(
        std::shared_ptr<Shape> shape,
        std::shared_ptr<Material> material,
        std::shared_ptr<Light> light = nullptr);

    virtual bool intersect(const Ray3f& ray3, hit_record& record) const override;

    virtual Bounds3f world_bound() const override;

    virtual void get_intersect_record(const Ray3f& ray3, hit_record& record) const override;

    virtual void compute_BxDF(hit_record& record) const override;
};
