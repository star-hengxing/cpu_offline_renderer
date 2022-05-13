#include <Core/Shape/Cuboid.hpp>

#include <Hinae/Transform.hpp>

#include <hit_record.hpp>
#include <util/util.hpp>

Cuboid::Cuboid(const Matrix4f& local_to_world, const Matrix4f& world_to_local
        , f32 length, f32 width, f32 height)
    : Shape(local_to_world, world_to_local)
    , length(length), width(width), height(height) {}

Vector3f Cuboid::get_normal(const Point3f& p) const
{
    Vector3f normal{0, 0, 0};
    const Axis max_axis = abs(as<Vector3, f32>(p) / Vector3{length, height, width}).max_dimension();
    normal[max_axis] = sign(p[max_axis]);
    return normal;
}

Bounds3f Cuboid::world_bound() const
{
    return local_to_world * local_bound();
}

Bounds3f Cuboid::local_bound() const
{
    return
    {
        {-length, -height, -width},
        { length,  height,  width}
    };
}

bool Cuboid::intersect(const Ray3f& ray3, hit_record& record) const
{
    const auto ray = world_to_local * ray3;
    const auto [x, y, z] = ray.direction;

    const Vector3f inv{1 / x, 1 / y, 1 / z};
    const auto [p1, p2] = local_bound();
    const Vector3f min_t = (p1 - ray.origin) * inv;
    const Vector3f max_t = (p2 - ray.origin) * inv;

    const auto enter = min(min_t, max_t).max_component();
    const auto exit = max(min_t, max_t).min_component();
    if(enter > exit) return false;
    return record.set_t(enter);
}

void Cuboid::get_intersect_record(const Ray3f& ray3, hit_record& record) const
{
    record.p = ray3.at(record.t_min);
    const auto local_p = world_to_local * record.p;
    record.n = local_to_world * get_normal(local_p);
}

f32 Cuboid::area() const
{
    const auto front = 2 * length * height;
    const auto right = 2 * width * height;
    const auto top   = 2 * length * width;
    return front + right + top;
}

std::tuple<Point3f, Vector3f, f32> Cuboid::sample(const Point2f& random) const
{
    unimplemented();
}

f32 Cuboid::pdf() const
{
    unimplemented();
}
