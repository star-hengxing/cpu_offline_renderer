#include <Core/Shape/Rectangle.hpp>

#include <Hinae/Transform.hpp>

#include <hit_record.hpp>
#include <global.hpp>

Rectangle::Rectangle(const Matrix4f& local_to_world, const Matrix4f& world_to_local
    , f32 length, f32 width)
    : Shape(local_to_world, world_to_local)
    , length(length), width(width) {}

Vector3f Rectangle::get_local_normal() const
{
    return {0, 1, 0};
}

Vector3f Rectangle::get_world_normal() const
{
    // local_to_world * get_local_normal()
    return local_to_world.column(1);
}

Point2f Rectangle::get_uv(const Point2f& p) const
{
    const auto [x, z] = p;
    return {(x + length) / (2 * length), (z + width) / (2 * width)};
}

Bounds3f Rectangle::world_bound() const 
{
    return local_to_world * local_bound();
}

Bounds3f Rectangle::local_bound() const 
{
    return
    {
        {-length, local_y, -width},
        { length, local_y,  width}
    };
}

bool Rectangle::intersect(const Ray3f& ray3, hit_record& record) const 
{
    const auto ray = world_to_local * ray3;
    if(is_zero(ray.direction.y)) return false;
    const auto t = ray.inv_at<Axis::Y>(local_y);
    return record.set_t(t);
}

void Rectangle::get_intersect_record(const Ray3f& ray3, hit_record& record) const
{
    record.p  = ray3.at(record.t_min);
    auto normal  = get_world_normal();
    if(is_same_direction(normal, ray3.direction)) normal = -normal;
    record.n = normal;
    const auto local_p = world_to_local * record.p;
    record.uv = get_uv({local_p.x, local_p.z});
}

f32 Rectangle::area() const
{
    return (length * length) * (width + width);
}

std::tuple<Point3f, Vector3f, f32> Rectangle::sample(const Point2f& random) const
{
    const auto [x, z] = random;
    const Point3f p
    {
        length * (x * 2 - 1),
        local_y,
        width * (z * 2 - 1)
    };

    return
    {
        local_to_world * p,
        get_world_normal(),
        pdf()
    };
}

f32 Rectangle::pdf() const
{
    return 1 / area();
}
