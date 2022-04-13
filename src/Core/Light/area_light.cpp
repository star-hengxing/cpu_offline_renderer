#include <Core/Light/area_light.hpp>

#include <Core/Shape/Shape.hpp>
#include <hit_record.hpp>

area_light::area_light(const std::shared_ptr<Shape>& shape, const Spectrum& emit)
    : Light(Flags::area)
    , shape(shape), emit(emit), area(shape->area()) {}

Spectrum area_light::Li(const hit_record& record, const Vector3f& w) const
{
    return dot(record.n, w) > 0 ? emit : Spectrum{0};
}

std::tuple<Vector3f, f32> area_light::get_dir_and_distance(const Point3f& p) const
{
    return {{0}, 0};
}

std::tuple<Vector3f, f32, f32> area_light::sample_li(const Point3f& p, const Point2f& random) const
{
    const auto [random_p, pdf] = shape->sample(random);
    const Vector3f direction = (random_p - p).normalized();
    const f32 t = distance(p, random_p);
    return {direction, t, pdf};
}

f32 area_light::pdf_li(const Point3f& p) const
{
    return 1 / area;
}
