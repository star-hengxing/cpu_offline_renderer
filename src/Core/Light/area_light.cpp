#include <Core/Light/area_light.hpp>

#include <Core/Shape/Shape.hpp>
#include <hit_record.hpp>

area_light::area_light(const std::shared_ptr<Shape>& shape, const Spectrum& emit)
    : Light(Flags::area)
    , shape(shape), emit(emit), area(shape->area()) {}

Spectrum area_light::Li(const Vector3f& normal, const Vector3f& emit_dir) const
{
    return dot(normal, emit_dir) > 0 ? emit : Spectrum{0};
}

Spectrum area_light::Li(const hit_record& record, const Vector3f& w) const
{
    return Li(record.n, w);
}

std::optional<light_sample> area_light::sample_li(const Point3f& p, const Point2f& random) const
{
    const auto [light_p, light_n, pdf] = shape->sample(random);
    const Vector3f direction = (light_p - p).normalized();
    const f32 t = distance(p, light_p);
    return light_sample{direction, light_n, t, pdf, Li(light_n, -direction)};
}

f32 area_light::pdf_li(const Point3f& p) const
{
    return 1 / area;
}
