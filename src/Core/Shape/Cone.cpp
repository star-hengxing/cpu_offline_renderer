#include <Core/Shape/Cone.hpp>

#include <Hinae/Transform.hpp>
#include <Hinae/equation.hpp>

#include <hit_record.hpp>
#include <util/util.hpp>

Cone::Cone(const Matrix4f& local_to_world, const Matrix4f& world_to_local
    , f32 radius, f32 max_height, f32 max_angle)
    : Shape(local_to_world, world_to_local)
    , radius(radius), max_height(max_height)
    , max_angle(to_radian(max_angle)) {}

Vector3f Cone::get_normal(f32 x, f32 z) const
{
    const auto r = std::sqrt(x * x + z * z);
    return Vector3{x, r * (radius / max_height), z}.normalized();
}

Point2f Cone::get_uv(f32 phi, f32 y) const
{
    return
    {
        phi / max_angle,
        y / max_height
    };
}

Bounds3f Cone::world_bound() const 
{
    return local_to_world * local_bound();
}

Bounds3f Cone::local_bound() const 
{
    return
    {
        {-radius, min_height, -radius},
        { radius, max_height,  radius}
    };
}

bool Cone::intersect(const Ray3f& ray3, hit_record& record) const 
{
    const auto ray = world_to_local * ray3;
    const auto [ox, oy, oz] = ray.origin;
    const auto [dx, dy, dz] = ray.direction;
    const auto k = pow2(radius / max_height);

    const auto a = dx * dx + dz * dz - k * dy * dy;
    const auto b = 2 * (ox * dx + oz * dz - k * dy * (oy - max_height));
    const auto c = ox * ox + oz * oz - k * pow2(oy - max_height);

    const auto result = quadratic(a, b, c);
    if(!result) return false;

    auto [x1, x2] = result.value();
    if(x1 > x2) std::swap(x1, x2);

    // if surface not closed, should flip normal which inside the curve
    auto inside = false;
    // TODO: clean code
    Point3f p;
    f32 t, phi;
    auto eval_point_and_phi = [&](f32 tmp_t)
    {
        t = tmp_t;
        p = ray.at(t);
        phi = std::atan2(p.z, p.x);
        if(phi < 0) phi += 2 * PI<f32>;
    };
    eval_point_and_phi(x1);
    if(p.y < min_height || p.y > max_height || phi > max_angle)
    {
        eval_point_and_phi(x2);
        if(p.y < min_height || p.y > max_height || phi > max_angle)
            return false;
        else
            inside = true;
    }

    const auto updated = record.set_t(t);
    if(updated)
    {
        record.n = get_normal(p.x, p.z);
        if(inside) record.n = -record.n;
        record.uv = get_uv(phi, p.y);
    }
    return updated;
}

void Cone::get_intersect_record(const Ray3f& ray3, hit_record& record) const
{
    record.p = ray3.at(record.t_min);
    record.n = local_to_world * record.n;
}

f32 Cone::area() const
{
    return PI<f32> * radius * std::sqrt(pow2(radius) + pow2(max_height));
}

std::tuple<Point3f, Vector3f, f32> Cone::sample(const Point2f& random) const
{
    unimplemented();
}

f32 Cone::pdf() const
{
    unimplemented();
}
