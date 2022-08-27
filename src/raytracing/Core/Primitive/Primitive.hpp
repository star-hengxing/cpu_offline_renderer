#pragma once

#include <math/Matrix4.hpp>
#include <math/Bounds3.hpp>
#include <math/Ray3.hpp>

using Spectrum = Vector3f;

struct hit_record;

struct Primitive
{
public:
    virtual bool intersect(const Ray3f& ray3, hit_record& record) const = 0;

    virtual Bounds3f world_bound() const = 0;

    virtual void get_intersect_record(const Ray3f& ray3, hit_record& record) const = 0;

    virtual void compute_BxDF(hit_record& record) const = 0;
};
