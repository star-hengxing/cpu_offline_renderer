#include <raytracing/Core/Shape/Annulus.hpp>

#include <math/Transform.hpp>

#include <raytracing/hit_record.hpp>
#include <raytracing/Sample.hpp>

Annulus::Annulus(const Matrix4f& local_to_world, const Matrix4f& world_to_local
    , f32 major_radius, f32 minor_radius, f32 max_angle)
    : Shape(local_to_world, world_to_local)
    , major_radius(major_radius), minor_radius(minor_radius)
    , max_angle(to_radian(max_angle)) {}

Vector3f Annulus::get_local_normal() const
{
    return {0, 1, 0};
}

Vector3f Annulus::get_world_normal() const
{
    // local_to_world * get_local_normal()
    return local_to_world.column(1);
}

Point2f Annulus::get_uv(f32 phi, f32 radius) const
{
    return
    {
        phi / max_angle,
        1 - (radius - minor_radius) / (major_radius - minor_radius)
    };
}

Bounds3f Annulus::world_bound() const 
{
    return local_to_world * local_bound();
}

Bounds3f Annulus::local_bound() const 
{
    return
    {
        {-major_radius, local_y, -major_radius},
        { major_radius, local_y,  major_radius}
    };
}

bool Annulus::intersect(const Ray3f& ray3, hit_record& record) const 
{
    const auto ray = world_to_local * ray3;
    if(is_zero(ray.direction.y)) return false;

    const auto t = ray.inv_at<Axis::Y>(local_y);

    const auto [x, y, z] = ray.at(t);
    const auto r = x * x + z * z;
    if(r > pow2(major_radius) || r < pow2(minor_radius))
        return false;

    auto phi = std::atan2(z, x);
    if(phi < 0) phi += 2 * PI<f32>;
    if(phi > max_angle) return false;

    record.uv = get_uv(phi, std::sqrt(r));
    return record.set_t(t);
}

void Annulus::get_intersect_record(const Ray3f& ray3, hit_record& record) const
{
    record.p = ray3.at(record.t_min);
    record.n = get_world_normal();
}

f32 Annulus::area() const
{
    // max_angle / 2PI * PI * (major_radius^2 - minor_radius^2)
    return (max_angle / 2) * (pow2(major_radius) - pow2(minor_radius));
}

std::tuple<Point3f, Vector3f, f32> Annulus::sample(const Point2f& random) const
{
    // [0, 1] => [0, R - r] => [r, R]
    const auto radius = lerp(minor_radius, major_radius, std::sqrt(random.x));
    const auto theta  = random.y * max_angle;
    const auto [x, z] = Point2{radius * std::cos(theta), radius * std::sin(theta)};
    return
    {
        local_to_world * Point3{x, local_y, z},
        get_world_normal(),
        pdf()
    };
}

f32 Annulus::pdf() const
{
    return 1 / area();
}
