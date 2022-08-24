#include <raytracing/Core/Light/point_light.hpp>

#include <raytracing/hit_record.hpp>

point_light::point_light(const Point3f& position, const Vector3f& intensity)
    : Light(Flags::delta_position)
    , position(position), intensity(intensity) {}

Spectrum point_light::Li(const Point3f& p) const
{
    return intensity / (position - p).norm2();
}

Spectrum point_light::Li(const hit_record& record, const Vector3f& w) const
{
    return Li(record.p);
}

std::optional<light_sample> point_light::sample_li(const Point3f& p, const Point2f& random) const
{
    const Vector3f direction = (position - p).normalized();
    const f32 t = distance(p, position);
    const f32 pdf = 1;
    return light_sample{direction, {0}, t, pdf, Li(p)};
}

f32 point_light::pdf_li(const Point3f& p) const
{
    return 0;
}
