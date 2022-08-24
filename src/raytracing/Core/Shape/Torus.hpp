#pragma once

#include <memory>

#include "Shape.hpp"

struct Torus : public Shape
{
private:
    f32 major_radius, minor_radius;

    Vector3f get_normal(const Point3f& p) const;

    Point2f get_uv(const Point3f& p) const;

public:
    Torus(const Matrix4f& local_to_world, const Matrix4f& world_to_local, f32 major_radius, f32 minor_radius);

    virtual Bounds3f world_bound() const override;

    virtual Bounds3f local_bound() const override;

    virtual bool intersect(const Ray3f& ray3, hit_record& record) const override;

    virtual void get_intersect_record(const Ray3f& ray3, hit_record& record) const override;
    
    virtual f32 area() const override;

    virtual std::tuple<Point3f, Vector3f, f32> sample(const Point2f& random) const override;

    virtual f32 pdf() const override;
};
