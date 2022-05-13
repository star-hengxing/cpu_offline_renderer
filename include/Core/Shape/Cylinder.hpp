#pragma once

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
    static constexpr f32 min_height = 0;

    const f32 radius;
    const f32 max_height;
    const f32 max_angle;
    max_shape cylinder_shape;

    Vector3f get_normal(f32 x, f32 z) const;

    Point2f get_uv(f32 phi, f32 y) const;

    bool surface_intersect(const Ray3f& ray, hit_record& record, f32 cap_t) const;

    std::tuple<f32, f32> cap_intersect(const Ray3f& ray) const;

    bool check_cap(const Ray3f& ray, f32 t) const;

public:
    Cylinder(const Matrix4f& local_to_world, const Matrix4f& world_to_local
        , f32 radius, f32 max_height, f32 max_angle, max_shape cylinder_shape);

    virtual Bounds3f world_bound() const override;

    virtual Bounds3f local_bound() const override;

    virtual bool intersect(const Ray3f& ray3, hit_record& record) const override;
    
    virtual void get_intersect_record(const Ray3f& ray3, hit_record& record) const override;
    
    virtual f32 area() const override;

    virtual std::tuple<Point3f, Vector3f, f32> sample(const Point2f& random) const override;

    virtual f32 pdf() const override;
};
