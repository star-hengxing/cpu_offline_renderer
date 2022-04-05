#pragma once

#include <memory>

#include "Light.hpp"

struct point_light : public Light
{
protected:
    Point3f position;
    Vector3f intensity;

public:
    point_light(const Point3f& position, const Vector3f& intensity);

    virtual Spectrum Li(const Point3f& p) const;

    virtual std::tuple<Vector3f, f32> get_dir_and_distance(const Point3f& p) const;
};
