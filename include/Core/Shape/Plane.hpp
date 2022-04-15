#pragma once

#include <memory>

#include "Shape.hpp"

struct Plane : public Shape
{
private:
    static constexpr auto local_y = ZERO<f32>;
    const f32 length;

    Vector3f get_normal() const;

public:
    Plane(const Matrix4f& local_to_world, const Matrix4f& world_to_local, f32 length);

    virtual Bounds3f world_bound() const override;

    virtual Bounds3f local_bound() const override;

    virtual bool intersect(const Ray3f& ray3, hit_record& record) const override;
    
    virtual void get_intersect_record(const Ray3f& ray3, hit_record& record) const override;
    
    virtual f32 area() const;

    virtual std::tuple<Point3f, Vector3f, f32> sample(const Point2f& random) const;

    virtual f32 pdf(const Point2f& random) const;
};
