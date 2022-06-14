#include <Core/Shape/Torus.hpp>

#include <Hinae/Transform.hpp>

#include <hit_record.hpp>
#include <util/util.hpp>

Torus::Torus(const Matrix4f& local_to_world, const Matrix4f& world_to_local, f32 major_radius, f32 minor_radius)
    : Shape(local_to_world, world_to_local)
    , major_radius(major_radius), minor_radius(minor_radius) {}

Vector3f Torus::get_normal(const Point3f& p) const
{
    const auto [x, y, z] = p;
    // n = p - o, o = w * p, n = (1 - w) * p
    const auto w = 1 - std::sqrt(x * x + z * z) / major_radius;
    return Vector3{w * x, y, w * z}.normalized();
}

Point2f Torus::get_uv(const Point3f& p) const
{
    unimplemented();
}

Bounds3f Torus::world_bound() const
{
    return local_to_world * local_bound();
}

Bounds3f Torus::local_bound() const
{
    const auto length = major_radius + minor_radius;
    return
    {
        {-length, -1, -length},
        { length,  1,  length}
    };
}

bool Torus::intersect(const Ray3f& ray3, hit_record& record) const
{
    const auto ray = world_to_local * ray3;
    const auto o = as<Vector3, f32>(ray.origin);
    const auto oy = o.y;
    const auto dy = ray.direction.y;
    const auto R2 = pow2(major_radius);
    const auto r2 = pow2(minor_radius);
    const auto d2 = ray.direction.norm2();
    const auto o2 = o.norm2();
    const auto od = dot(ray.direction, o);
    const auto od2 = od * od;

    const auto a4 = d2 * d2;
    const auto a3 = 4 * d2 * od;
    const auto a2 = 2 * (o2 - r2 - R2) * d2 + 4 * od2 + 4 * R2 * pow2(dy);
    const auto a1 = 4 * (o2 - r2 - R2) * od + 8 * R2 * oy * dy;
    const auto a0 = pow2(o2 - r2 - R2) - 4 * R2 * (r2 - pow2(oy));
    // cubic equation: y^3+b2*y^2+b1*y+b0 = 0
    const auto y = [](f32 b2, f32 b1, f32 b0) -> f32
    {
        const auto p = b1 / 3 - pow2(b2) / 9;
        const auto q = pow3(b2) / 27 - b1 * b2 / 6 + b0 / 2;
        const auto deta = pow2(q) + pow3(p);
        f32 tmp;
        if(deta > 0)
        {
            tmp = std::cbrt(-q + std::sqrt(deta)) + std::cbrt(-q - std::sqrt(deta));
        }
        else if(is_zero(deta))
        {
            tmp = -2 * std::cbrt(q);
        }
        else // deta < 0
        {
            const auto alpha = std::acos(-q * std::sqrt(-p) / pow2(p)) / 3;
            tmp = 2 * std::sqrt(-p) * std::cos(alpha);
        }
        return tmp - b2 / 3;
    }(-a2, a3 * a1 - 4 * a0, 4 * a2 * a0 - pow2(a1) - pow2(a3) * a0);
    const auto R = std::sqrt(pow2(a3) / 4 - a2 + y);
    f32 D, E;
    if(is_zero(R))
    {
        D = std::sqrt((3.0f / 4) * pow2(a3) - 2 * a2 + 2 * std::sqrt(pow2(y) - 4 * a0));
        E = std::sqrt((3.0f / 4) * pow2(a3) - 2 * a2 - 2 * std::sqrt(pow2(y) - 4 * a0));
    }
    else
    {
        D = std::sqrt((3.0f / 4) * pow2(a3) - pow2(R) - 2 * a2 + (4 * a3 * a2 - 8 * a1 - pow3(a3) / (4 * R)));
        E = std::sqrt((3.0f / 4) * pow2(a3) - pow2(R) - 2 * a2 - (4 * a3 * a2 - 8 * a1 - pow3(a3) / (4 * R)));
    }
    const auto x1 = -a3 / 4 + R / 2 + D / 2;
    const auto x2 = -a3 / 4 + R / 2 - D / 2;
    const auto x3 = -a3 / 4 - R / 2 + E / 2;
    const auto x4 = -a3 / 4 - R / 2 - E / 2;
    auto t = INFINITY_<f32>;
    for(auto x : {x1, x2, x3, x4})
    {
        if(x > 0 && x < t)
        {
            t = x;
        }
    }
    return record.set_t(t);
}

void Torus::get_intersect_record(const Ray3f& ray3, hit_record& record) const
{
    record.p = ray3.at(record.t_min);
    const Point3f local_p = world_to_local * record.p;
    record.n = local_to_world * get_normal(local_p);
    // record.uv = get_uv(local_p);
}

f32 Torus::area() const
{
    return 4 * pow2(PI<f32>) * major_radius * minor_radius;
}

std::tuple<Point3f, Vector3f, f32> Torus::sample(const Point2f& random) const
{
    unimplemented();
}

f32 Torus::pdf() const
{
    unimplemented();
}
