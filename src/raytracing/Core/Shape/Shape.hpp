#pragma once

#include <math/Matrix4.hpp>
#include <math/Bounds3.hpp>
#include <math/Point3.hpp>
#include <math/Point2.hpp>
#include <math/Ray3.hpp>

struct hit_record;

struct Shape
{
protected:
    const Matrix4f& local_to_world;
    const Matrix4f& world_to_local;

    Shape(const Matrix4f& local_to_world, const Matrix4f& world_to_local)
        : local_to_world(local_to_world)
        , world_to_local(world_to_local) {}

public:
    virtual ~Shape() = default;

    virtual bool intersect(const Ray3f& ray3, hit_record& record) const = 0;

    virtual Bounds3f world_bound() const = 0;

    virtual Bounds3f local_bound() const = 0;

    virtual void get_intersect_record(const Ray3f& ray3, hit_record& record) const = 0;
    
    virtual f32 area() const = 0;

    virtual std::tuple<Point3f, Vector3f, f32> sample(const Point2f& random) const = 0;

    virtual f32 pdf() const = 0;
};
