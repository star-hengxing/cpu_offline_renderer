#pragma once

#include <memory>

#include "Shape.hpp"

struct Sphere : public Shape
{
private:
    f32 radius;

public:
    Sphere(const Matrix4f& local_to_world, const Matrix4f& world_to_local, f32 radius);

    virtual Bounds3f world_bound() const override;

    virtual Bounds3f local_bound() const override;

    virtual bool intersect(const Ray3f& ray3, hit_record& record) const override;

    virtual void get_intersect_record(const Ray3f& ray3, hit_record& record) const override;
};
