#pragma once

#include "point_light.hpp"

struct spot_light : public point_light
{
private:
    const f32 cos_width, cos_falloff;
    const Vector3f dir;

public:
    spot_light(const Point3f& position
        , const Vector3f& intensity
        , f32 cos_width, f32 cos_falloff
        , const Vector3f& dir);

    virtual Spectrum Li(const Point3f& p) const;

    f32 falloff(const Vector3f& v) const;
};
