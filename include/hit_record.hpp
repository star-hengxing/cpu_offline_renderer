#pragma once

#include <Hinae/Vector3.hpp>
#include <Hinae/Point3.hpp>
#include <Hinae/Point2.hpp>
#include <Hinae/Ray3.hpp>

using namespace Hinae;

struct BxDF;

struct hit_record
{
    f32 t_min = INFINITY_<f32>;
    Point3f p;
    Vector3f n;
    Point2f uv;
    BxDF* bxdf;

    hit_record() = default;

    bool is_hit() const;

    bool set_t(f32 t);

    Ray3f new_ray(const Vector3f& direction) const;
};
