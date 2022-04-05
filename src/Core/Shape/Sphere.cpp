#include <Core/Shape/Sphere.hpp>

#include <Hinae/Transform.hpp>
#include <Hinae/equation.hpp>

#include <hit_record.hpp>

Sphere::Sphere(const Matrix4f& local_to_world, const Matrix4f& world_to_local, f32 radius)
    : Shape(local_to_world, world_to_local)
    , radius(radius) {}

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
    f32 a, b, c;
    const auto p = as<Vector3, f32>(ray.origin);
    a = 1;
    b = 2 * dot(p, ray.direction);
    c = p.norm2() - radius * radius;

    const auto result = quadratic(a, b, c);
    if(!result) return false;

    const auto [x1, x2] = result.value();

    const f32 t = min(x1, x2);
    
    return record.set_t(t);
}

void Sphere::get_intersect_record(const Ray3f& ray3, hit_record& record) const
{
    record.p = ray3.at(record.t_min);
    const Ray3f ray = world_to_local * ray3;
    const Point3f local_p = ray.at(record.t_min);
    // normalize
    const auto normal = as<Vector3, f32>(local_p) / radius;
    record.n = local_to_world * normal;

    const auto v = as<Vector3, f32>(local_p).normalized();
    const f32 theta = std::acos(clamp(-1.0f, v.z, 1.0f));
    const f32 p = std::atan2(v.y, v.x);
    const f32 phi = (p < 0) ? (p + 2 * PI<f32>) : p;
    record.uv = {theta * INV_PI<f32>, phi * INV_2PI<f32>};
}
