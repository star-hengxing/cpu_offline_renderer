#include <Core/Light/point_light.hpp>

point_light::point_light(const Point3f& position, const Vector3f& intensity)
    : position(position), intensity(intensity) {}

Spectrum point_light::Li(const Point3f& p) const
{
    return intensity / (position - p).norm2();
}

std::tuple<Vector3f, f32> point_light::get_dir_and_distance(const Point3f& p) const
{
    const Vector3f direction = (position - p).normalized();
    const f32 t = distance(p, position);
    return {direction, t};   
}
