#pragma once

#include <math/Vector3.hpp>
#include <math/Point3.hpp>
#include <math/Point2.hpp>
#include <math/Ray3.hpp>

#include <raytracing/Core/BSDF.hpp>

struct Light;

struct hit_record
{
    f32      t_min = INFINITY_<f32>;
    f32      t_max = INFINITY_<f32>;
    Point3f  p;
    Vector3f n;
    Point2f  uv;
    BSDF     bsdf;
    Light*   light;

    hit_record() {}

    bool is_hit() const;

    bool is_light() const;

    bool set_t(f32 t);

    Ray3f new_ray(const Vector3f& direction) const;
};
