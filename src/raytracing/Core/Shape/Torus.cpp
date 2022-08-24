#include <raytracing/Core/Shape/Torus.hpp>

#include <math/Transform.hpp>

#include <raytracing/hit_record.hpp>
#include <util/util.hpp>

Torus::Torus(const Matrix4f& local_to_world, const Matrix4f& world_to_local, f32 major_radius, f32 minor_radius)
    : Shape(local_to_world, world_to_local)
    , major_radius(major_radius), minor_radius(minor_radius) {}

Vector3f Torus::get_normal(const Point3f& p) const
{
    const auto [x, y, z] = p;
    const auto a = major_radius / std::sqrt(pow2(x) + pow2(z)) - 1;
    return Vector3{x * a, y, z * a}.normalized();
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

static std::array<f32, 2> solve2(f32 a, f32 b, f32 c)
{
    const auto p = b / (2 * a);
    const auto q = c / a;
    auto det = p * p - q;

    if (is_zero(det))
    {
        return {-p, INFINITY_<f32>};
    }
    else if (det < 0)
    {
        return {INFINITY_<f32>, INFINITY_<f32>};
    }
    else
    {
        det = std::sqrt(det);
        return {det - p, -det - p};
    }
}

// Cardano formula
static auto cubic(f32 a, f32 b, f32 c, f32 d)
{
    const auto inv = reciprocal(a);
    const auto A = b * inv;
    const auto B = c * inv;
    const auto C = d * inv;

    const auto A2 = A * A;
    const auto p = (B - A2 / 3) / 3;
    const auto q = (2.0f / 27 * A * A2 - A * B / 3 + C) / 2;

    const auto p3 = pow3(p);
    auto det = q * q + p3;

    auto s = std::array<f32, 3>{};

    if (is_zero(det))
    {
        if (is_zero(q))
        {
            s.fill(0);
        }
        else
        {
            const auto u = std::cbrt(q);
            s = {-2 * u, u, u};
        }
    }
    else if (det < 0)
    {
        const auto phi = std::acos(-q / std::sqrt(-p3)) / 3;
        const auto tmp = 2 * std::sqrt(-p);
        s =
        {
             tmp * std::cos(phi),
            -tmp * std::cos(phi + PI<f32> / 3),
            -tmp * std::cos(phi - PI<f32> / 3)
        };
    }
    else
    {
        det = std::sqrt(det);
        const auto u = std::cbrt( det - q);
        const auto v = std::cbrt(-det - q);
        s = {u + v, INFINITY_<f32>, INFINITY_<f32>};
    }

    const auto sub = A / 3;
    for (auto i : range(s.size()))
        s[i] -= sub;

    return s;
}

static std::array<f32, 4> quartic(f32 b, f32 c, f32 d, f32 e)
{
    const auto A = b;
    const auto B = c;
    const auto C = d;
    const auto D = e;

    const auto A2 = pow2(A);
    const auto p = -3.0f * A2 / 8 + B;
    const auto q = A2 * A / 8 - A * B / 2 + C;
    const auto r = -3.0f / 256 * A2 * A2 + A2 * B / 16 - A * C / 4 + D;

    std::array<f32, 4> ret;

    if(is_zero(r))
    {
        auto value = cubic(1, 0, p, q);
        ret = {value[0], value[1], value[2], 0};
    }
    else
    {
        auto s = cubic(1, -p / 2, -r, r * p / 2 - q * q / 8);
        auto z = s[0];

        // to build two quadric equations
        auto u = z * z - r;
        auto v = 2 * z - p;

        if (is_zero(u))
            u = 0;
        else if (u > 0)
            u = std::sqrt(u);
        else
            return {INFINITY_<f32>};

        if (is_zero(v))
            v = 0;
        else if (v > 0)
            v = std::sqrt(v);
        else
            return {INFINITY_<f32>};

        auto tmp2 = solve2(1, q < 0 ? -v :  v, z - u);
        auto tmp3 = solve2(1, q < 0 ?  v : -v, z + u);
        ret = {tmp2[0], tmp2[1], tmp3[0], tmp3[1]};
    }

    const auto sub = A / 3;
    for (auto i : range(ret.size()))
        ret[i] -= sub;

    return ret;
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

    const auto result = quartic(b, c, d, e);

    auto t = INFINITY_<f32>;
    for(const auto value : result)
    {
        if(value > 0.001f && value < t)
            t = value;
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