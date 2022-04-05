#include <Core/Shape/Cube.hpp>

#include <hit_record.hpp>
#include <Hinae/Transform.hpp>

Cube::Cube(const Matrix4f& local_to_world, const Matrix4f& world_to_local)
    : Shape(local_to_world, world_to_local) {}

Vector3f Cube::get_normal(const Point3f& p) const
{
    Vector3f normal{0, 0, 0};
    const Axis max_axis = abs(as<Vector3, f32>(p)).max_dimension();
    normal[max_axis] = p[max_axis];
    return normal;
}

Bounds3f Cube::world_bound() const 
{
    return local_to_world * local_bound();
}

Bounds3f Cube::local_bound() const 
{
    return
    {
        {-length, -length, -length},
        { length,  length,  length}
    };
}

bool Cube::intersect(const Ray3f& ray3, hit_record& record) const 
{
    const auto ray = world_to_local * ray3;
    const auto [x, y, z] = ray.direction;
    const Vector3f inv{1 / x, 1 / y, 1 / z};

    const Vector3f min_t = (Point3f{-length} - ray.origin) * inv;
    const Vector3f max_t = (Point3f{length} - ray.origin) * inv;

    const f32 t = min(min_t, max_t).max_component();
    return record.set_t(t);
}

void Cube::get_intersect_record(const Ray3f& ray3, hit_record& record) const
{
    record.p = ray3.at(record.t_min);
    const Point3f local_point = world_to_local * record.p;
    record.n = local_to_world * get_normal(local_point);
}
