#pragma once

#include <memory>

#include "Shape.hpp"

struct Cube : public Shape
{
private:
    static constexpr auto length  = ONE<f32>;

    Vector3f get_normal(const Point3f& p) const;

public:
    Cube(const Matrix4f& local_to_world, const Matrix4f& world_to_local);

    virtual Bounds3f world_bound() const override;

    virtual Bounds3f local_bound() const override;

    virtual bool intersect(const Ray3f& ray3, hit_record& record) const override;
    
    virtual void get_intersect_record(const Ray3f& ray3, hit_record& record) const override;
};
