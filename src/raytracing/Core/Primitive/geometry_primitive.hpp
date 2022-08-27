#pragma once

#include "Primitive.hpp"

struct Shape;
struct Light;
struct Material;

struct geometry_primitive : public Primitive
{
private:
    Shape*    shape;
    Material* material;
    Light*    light;

public:
    geometry_primitive() {}

    geometry_primitive(Shape* shape, Material* material, Light* light = nullptr);

    virtual bool intersect(const Ray3f& ray3, hit_record& record) const override;

    virtual Bounds3f world_bound() const override;

    virtual void get_intersect_record(const Ray3f& ray3, hit_record& record) const override;

    virtual void compute_BxDF(hit_record& record) const override;
};
