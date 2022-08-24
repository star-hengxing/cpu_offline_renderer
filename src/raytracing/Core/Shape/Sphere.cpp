#include <raytracing/Core/Shape/Sphere.hpp>

#include <math/Transform.hpp>
#include <math/equation.hpp>

#include <raytracing/hit_record.hpp>
#include <raytracing/Sample.hpp>

Sphere::Sphere(const Matrix4f& local_to_world, const Matrix4f& world_to_local, f32 radius)
    : Shape(local_to_world, world_to_local)
    , radius(radius) {}

Vector3f Sphere::get_normal(const Point3f& p) const
{
    return as<Vector3, f32>(p) / radius;
}

Point2f Sphere::get_uv(const Point3f& p) const
{
    const auto [x, y, z] = as<Vector3, f32>(p) / radius;
    const auto theta = std::acos(y);
    auto phi = std::atan2(z, x);
    if(phi < 0) phi += 2 * PI<f32>;
    return
    {
        phi   * INV_2PI<f32>,
        theta * INV_PI<f32>
    };
}

Bounds3f Sphere::world_bound() const
{
    return local_to_world * local_bound();
}

Bounds3f Sphere::local_bound() const
{
    return
    {
        {-radius, -radius, -radius},
        { radius,  radius,  radius}
    };
}

bool Sphere::intersect(const Ray3f& ray3, hit_record& record) const
{
    const auto ray = world_to_local * ray3;
    const auto p = as<Vector3, f32>(ray.origin);
    // use normalize ray direction
    const auto a = 1.0f; // ray.direction.norm2()
    const auto b = 2 * dot(p, ray.direction);
    const auto c = p.norm2() - radius * radius;

    const auto result = quadratic(a, b, c);
    if(!result) return false;

    const auto [x1, x2] = *result;
    const auto t = min(x1, x2);
    
    return record.set_t(t);
}

void Sphere::get_intersect_record(const Ray3f& ray3, hit_record& record) const
{
    record.p = ray3.at(record.t_min);
    const auto local_p = world_to_local * record.p;
    record.n = local_to_world * get_normal(local_p);
    record.uv = get_uv(local_p);
}

f32 Sphere::area() const
{
    return 4 * PI<f32> * radius * radius;
}

std::tuple<Point3f, Vector3f, f32> Sphere::sample(const Point2f& random) const
{
    const auto normal = as<Vector3, f32>(Sample::uniform_sphere(random));
    const auto random_p = as<Point3, f32>(normal * radius);
    return
    {
        local_to_world * random_p,
        local_to_world * normal,
        pdf()
    };
}

f32 Sphere::pdf() const
{
    return 1 / area();
}
