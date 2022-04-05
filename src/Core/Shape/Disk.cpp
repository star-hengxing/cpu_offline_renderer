#include <Core/Shape/Disk.hpp>

#include <Hinae/Transform.hpp>

#include <hit_record.hpp>

Disk::Disk(const Matrix4f& local_to_world, const Matrix4f& world_to_local
    , f32 height, f32 radius, f32 inner_radius, f32 max_angle)
    : Shape(local_to_world, world_to_local)
    , height(height), radius(radius)
    , inner_radius(inner_radius), max_angle(max_angle) {}

Bounds3f Disk::world_bound() const 
{
    return local_to_world * local_bound();
}

Bounds3f Disk::local_bound() const 
{
    return
    {
        {-radius, 0, -radius},
        { radius, height, radius}
    };
}

bool Disk::intersect(const Ray3f& ray3, hit_record& record) const 
{
    const auto ray = world_to_local * ray3;
    if(is_zero(ray.direction.y)) return false;

    f32 t = ray.inv_at<Axis::Y>(height);
    if(t < 0.001f) return false;

    Point3f p = ray.at(t);
    f32 r = p.x * p.x + p.z * p.z;
    if(r > radius * radius || r < inner_radius * inner_radius)
        return false;
    f32 angle = std::abs(std::atan2(p.z, p.x));
    if(angle > max_angle) return false;

    return record.set_t(t);
}

void Disk::get_intersect_record(const Ray3f& ray3, hit_record& record) const
{
    record.p = ray3.at(record.t_min);
    record.n = local_to_world * Vector3f{0, 1, 0};
}
