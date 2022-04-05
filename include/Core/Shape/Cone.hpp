#pragma once

#include <memory>

#include "Shape.hpp"

struct Cone : public Shape
{
private:
    const f32 radius;
    const f32 height;
    const f32 max_angle;

public:
    Cone(const Matrix4f& local_to_world, const Matrix4f& world_to_local
        , f32 radius, f32 height, f32 max_angle);

    virtual Bounds3f world_bound() const override;

    virtual Bounds3f local_bound() const override;

    virtual bool intersect(const Ray3f& ray3, hit_record& record) const override;
    
    virtual void get_intersect_record(const Ray3f& ray3, hit_record& record) const override;
};
