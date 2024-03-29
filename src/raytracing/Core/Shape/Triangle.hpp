#pragma once

#include <memory>

#include "Shape.hpp"

#include <raytracing/Core/Mesh/Mesh.hpp>

struct TriangleMesh;

struct Triangle : public Shape
{
private:
    std::shared_ptr<TriangleMesh> mesh;
    const Face& face;

public:
    Triangle(const Matrix4f& local_to_world, const Matrix4f& world_to_local,
        const std::shared_ptr<TriangleMesh>& mesh, const Face& face);

    virtual Bounds3f world_bound() const override;

    virtual Bounds3f local_bound() const override;

    virtual bool intersect(const Ray3f& ray3, hit_record& record) const override;

    virtual void get_intersect_record(const Ray3f& ray3, hit_record& record) const override;
    
    virtual f32 area() const override;

    virtual std::tuple<Point3f, Vector3f, f32> sample(const Point2f& random) const override;

    virtual f32 pdf() const override;
};
