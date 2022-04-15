#include <Core/Shape/Plane.hpp>

#include <Hinae/Transform.hpp>

#include <hit_record.hpp>

Plane::Plane(const Matrix4f& local_to_world, const Matrix4f& world_to_local, f32 length)
    : Shape(local_to_world, world_to_local), length(length) {}

Vector3f Plane::get_normal() const
{
    return {0, 1, 0};
}

Bounds3f Plane::world_bound() const 
{
    return local_to_world * local_bound();
}

Bounds3f Plane::local_bound() const 
{
    return
    {
        {-length, local_y, -length},
        { length, local_y,  length}
    };
}

bool Plane::intersect(const Ray3f& ray3, hit_record& record) const 
{
    const auto ray = world_to_local * ray3;
    if(is_zero(ray.direction.y)) return false;
    const f32 t = ray.inv_at<Axis::Y>(local_y);
    const auto [x, y, z] = ray.at(t);
    if(std::abs(x) > length || std::abs(z) > length) return false;
    return record.set_t(t);
}

void Plane::get_intersect_record(const Ray3f& ray3, hit_record& record) const
{
    record.p  = ray3.at(record.t_min);
    record.n  = local_to_world * get_normal();
    record.uv = {record.p.x, record.p.z};
}

f32 Plane::area() const
{
    return pow2(length + length);
}

std::tuple<Point3f, Vector3f, f32> Plane::sample(const Point2f& random) const
{
    const auto [x, z] = random;
    const Point3f p
    {
        length * (x * 2 - 1),
        local_y,
        length * (z * 2 - 1)
    };

    return
    {
        local_to_world * p,
        local_to_world * get_normal(),
        pdf(random)
    };
}

f32 Plane::pdf(const Point2f& random) const
{
    return 1 / area();
}
