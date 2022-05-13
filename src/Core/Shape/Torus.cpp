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
    const auto a = 1 - major_radius / std::sqrt(pow2(x) + pow2(z));
    return Vector3{a * x, y, a * z} / minor_radius;
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
    // const f32 a = pow2(ray.direction.norm2());
    const auto a = 1;
    const auto b = 4 * dot(o, ray.direction);
    const auto c = 2 * (o.norm2() - (pow2(major_radius) + pow2(minor_radius))) + 4 * pow2(dot(o, ray.direction)) + 4 * pow2(major_radius) * pow2(ray.direction.y);
    const auto d = 4 * (o.norm2() - (pow2(major_radius) + pow2(minor_radius))) * dot(o, ray.direction) + 8 * pow2(major_radius) * o.y * ray.direction.y;
    const auto e = pow2(o.norm2() - (pow2(major_radius) + pow2(minor_radius))) - 4 * pow2(major_radius) * (pow2(minor_radius) - pow2(o.y));
    // cubic equation: y^3+b2*y^2+b1*y+b0 = 0
    const auto y = [](f32 b2, f32 b1, f32 b0) -> f32
    {
        const auto p = b1 / 3 - pow2(b2) / 9;
        const auto q = pow3(b2) / 27 - b1 * b2 / 6 + b0 / 2;
        const auto deta = pow2(q) + pow3(p);
        f32 tmp;
        if(deta > 0)
        {
            tmp = std::pow(-q + std::sqrt(deta), 1.0f / 3) + std::pow(-q - std::sqrt(deta), 1.0f / 3);
        }
        else if(is_zero(deta))
        {
            tmp = -2 * std::pow(q, 1.0f / 3);
        }
        else // deta < 0
        {
            const auto alpha = std::acos(-q * std::sqrt(-p) / pow2(p)) / 3;
            tmp = 2 * std::sqrt(-p) * std::cos(alpha);
        }
        return tmp - b2 / 3;
    }(-c, b * d - 4 * e, 4 * c * e - pow2(d) - pow2(b) * e);
    const auto R = std::sqrt(pow2(b) / 4 - c + y);
    f32 D, E;
    if(is_zero(R))
    {
        D = std::sqrt((3.0f / 4) * pow2(b) - 2 * c + 2 * std::sqrt(pow2(y) - 4 * e));
        E = std::sqrt((3.0f / 4) * pow2(b) - 2 * c - 2 * std::sqrt(pow2(y) - 4 * e));
    }
    else
    {
        D = std::sqrt((3.0f / 4) * pow2(b) - pow2(R) - 2 * c + (4 * b * c - 8 * d - pow3(b) / (4 * R)));
        E = std::sqrt((3.0f / 4) * pow2(b) - pow2(R) - 2 * c - (4 * b * c - 8 * d - pow3(b) / (4 * R)));
    }
    const auto x1 = -b / 4 + R / 2 + D / 2;
    const auto x2 = -b / 4 + R / 2 - D / 2;
    const auto x3 = -b / 4 - R / 2 + E / 2;
    const auto x4 = -b / 4 - R / 2 - E / 2;
    f32 t = x1;
    for(auto x : {x2, x3, x4})
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
