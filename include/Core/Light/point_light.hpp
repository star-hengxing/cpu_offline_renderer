#pragma once

#include "Light.hpp"

struct point_light : public Light
{
protected:
    const Point3f position;
    const Vector3f intensity;

public:
    point_light(const Point3f& position, const Vector3f& intensity);

    virtual Spectrum Li(const hit_record& record, const Vector3f& w) const;

    virtual std::tuple<Vector3f, f32> get_dir_and_distance(const Point3f& p) const;

    virtual std::tuple<Vector3f, f32, f32> sample_li(const Point3f& p, const Point2f& random) const;

    virtual f32 pdf_li(const Point3f& p) const;
};
