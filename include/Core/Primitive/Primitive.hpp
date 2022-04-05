#pragma once

#include <Hinae/Matrix4.hpp>
#include <Hinae/Bounds3.hpp>
#include <Hinae/Ray3.hpp>

using namespace Hinae;

using Spectrum = Vector3f;

struct hit_record;

struct Primitive
{
public:
    virtual ~Primitive() = default;

    virtual bool intersect(const Ray3f& ray3, hit_record& record) const = 0;

    virtual Bounds3f world_bound() const = 0;

    virtual void get_intersect_record(const Ray3f& ray3, hit_record& record) const = 0;

    virtual void compute_BxDF(hit_record& record) const = 0;
};
