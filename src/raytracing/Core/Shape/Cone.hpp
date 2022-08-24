#pragma once

#include "Shape.hpp"

struct Cone : public Shape
{
private:
    static constexpr f32 min_height = 0;

    const f32 radius;
    const f32 max_height;
    const f32 max_angle;

    Vector3f get_normal(f32 x, f32 z) const;

    Point2f get_uv(f32 phi, f32 y) const;

public:
    Cone(const Matrix4f& local_to_world, const Matrix4f& world_to_local
        , f32 radius, f32 max_height, f32 max_angle);

    virtual Bounds3f world_bound() const override;

    virtual Bounds3f local_bound() const override;

    virtual bool intersect(const Ray3f& ray3, hit_record& record) const override;
    
    virtual void get_intersect_record(const Ray3f& ray3, hit_record& record) const override;
    
    virtual f32 area() const override;

    virtual std::tuple<Point3f, Vector3f, f32> sample(const Point2f& random) const override;

    virtual f32 pdf() const override;
};
