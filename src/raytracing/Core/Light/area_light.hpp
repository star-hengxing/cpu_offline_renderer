#pragma once

#include "Light.hpp"

struct Shape;

struct area_light : public Light
{
protected:
    const Shape* shape;
    const Spectrum emit;
    const f32 area;

public:
    area_light(const Shape* shape, const Spectrum& emit);

    virtual Spectrum Li(const Vector3f& normal, const Vector3f& emit_dir) const;

    virtual Spectrum Li(const hit_record& record, const Vector3f& w) const override;

    virtual std::optional<light_sample> sample_li(const Point3f& p, const Point2f& random) const override;

    virtual f32 pdf_li(const Point3f& p) const override;
};
