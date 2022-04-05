#pragma once

#include <memory>

#include "point_light.hpp"

struct spot_light : public point_light
{
private:
    f32 cos_width, cos_falloff;
    Vector3f dir;

public:
    spot_light(const Point3f& position
        , const Vector3f& intensity
        , f32 cos_width, f32 cos_falloff
        , const Vector3f& dir);

    virtual Spectrum Li(const Point3f& p) const;

    virtual std::tuple<Vector3f, f32> get_dir_and_distance(const Point3f& p) const;

    f32 falloff(const Vector3f& v) const;
};
