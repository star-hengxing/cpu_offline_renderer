#pragma once

#include "Shape.hpp"

struct Rectangle : public Shape
{
private:
    static constexpr auto local_y = ZERO<f32>;
    const f32 length, width;

    Vector3f get_local_normal() const;

    Vector3f get_world_normal() const;

    Point2f get_uv(const Point2f& p) const;

public:
    Rectangle(const Matrix4f& local_to_world, const Matrix4f& world_to_local
        , f32 length, f32 width);

    virtual Bounds3f world_bound() const override;

    virtual Bounds3f local_bound() const override;

    virtual bool intersect(const Ray3f& ray3, hit_record& record) const override;
    
    virtual void get_intersect_record(const Ray3f& ray3, hit_record& record) const override;
    
    virtual f32 area() const override;

    virtual std::tuple<Point3f, Vector3f, f32> sample(const Point2f& random) const override;

    virtual f32 pdf() const override;
};
