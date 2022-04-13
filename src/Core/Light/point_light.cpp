#include <Core/Light/point_light.hpp>

#include <hit_record.hpp>

point_light::point_light(const Point3f& position, const Vector3f& intensity)
    : Light(Flags::delta_position)
    , position(position), intensity(intensity) {}

Spectrum point_light::Li(const hit_record& record, const Vector3f& w) const
{
    return intensity / (position - record.p).norm2();
}

std::tuple<Vector3f, f32> point_light::get_dir_and_distance(const Point3f& p) const
{
    const Vector3f direction = (position - p).normalized();
    const f32 t = distance(p, position);
    return {direction, t};   
}

std::tuple<Vector3f, f32, f32> point_light::sample_li(const Point3f& p, const Point2f& random) const
{
    const auto [dir, t] = get_dir_and_distance(p);
    return {dir, t, 1};
}

f32 point_light::pdf_li(const Point3f& p) const
{
    return 0;
}
