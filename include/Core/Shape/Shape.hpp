#pragma once

#include <Hinae/Matrix4.hpp>
#include <Hinae/Bounds3.hpp>
#include <Hinae/Ray3.hpp>

using namespace Hinae;

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
};
