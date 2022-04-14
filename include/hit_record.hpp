#pragma once

#include <memory>

#include <Hinae/Vector3.hpp>
#include <Hinae/Point3.hpp>
#include <Hinae/Point2.hpp>
#include <Hinae/Ray3.hpp>

using namespace Hinae;

#include <Core/BSDF.hpp>

struct Light;

struct hit_record
{
    f32 t_min = INFINITY_<f32>;
    f32 t_max = INFINITY_<f32>;
    Point3f p;
    Vector3f n;
    Point2f uv;
    BSDF bsdf;
    std::shared_ptr<Light> light;

    hit_record() = default;

    bool is_hit() const;

    bool is_light() const;

    bool set_t(f32 t);

    Ray3f new_ray(const Vector3f& direction) const;
};
