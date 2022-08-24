#pragma once

#include "Light.hpp"

struct point_light : public Light
{
protected:
    const Point3f position;
    const Vector3f intensity;

public:
    point_light(const Point3f& position, const Vector3f& intensity);

    virtual Spectrum Li(const Point3f& p) const;

    virtual Spectrum Li(const hit_record& record, const Vector3f& w) const;

    virtual std::optional<light_sample> sample_li(const Point3f& p, const Point2f& random) const;

    virtual f32 pdf_li(const Point3f& p) const;
};
