#pragma once

#include <optional>
#include <memory>

#include "Shape.hpp"

struct Cylinder : public Shape
{
public:
    enum max_shape : u32
    {
        cylinder_empty  = 0b00,
        cylinder_top    = 0b01,
        cylinder_bottom = 0b10,
        cylinder_full   = 0b11
    };

private:
    const f32 height;
    const f32 radius;
    const f32 max_angle;
    max_shape cylinder_shape;

    bool check_cap(const Ray3f& ray, f32 t) const;

    std::optional<f32> check_cap(const Ray3f& ray) const;

public:
    Cylinder(const Matrix4f& local_to_world, const Matrix4f& world_to_local
        , f32 height, f32 radius, f32 max_angle
        , max_shape cylinder_shape);

    virtual Bounds3f world_bound() const override;

    virtual Bounds3f local_bound() const override;

    virtual bool intersect(const Ray3f& ray3, hit_record& record) const override;
    
    virtual void get_intersect_record(const Ray3f& ray3, hit_record& record) const override;
};
