#include <raytracing/Core/Shape/Cylinder.hpp>

#include <math/Transform.hpp>
#include <math/equation.hpp>

#include <raytracing/hit_record.hpp>
#include <raytracing/global.hpp>

Cylinder::Cylinder(const Matrix4f& local_to_world, const Matrix4f& world_to_local
    , f32 radius, f32 max_height, f32 max_angle, max_shape cylinder_shape)
    : Shape(local_to_world, world_to_local)
    , radius(radius), max_height(max_height)
    , max_angle(to_radian(max_angle))
    , cylinder_shape(cylinder_shape) {}

Vector3f Cylinder::get_normal(f32 x, f32 z) const
{
    const auto inv = reciprocal(radius);
    return {x * inv, 0, z * inv};
}

Point2f Cylinder::get_uv(f32 phi, f32 y) const
{
    return
    {
        phi / max_angle,
        y / max_height // (y - min_height) / (max_height - min_height)
    };
}

Bounds3f Cylinder::world_bound() const 
{
    return local_to_world * local_bound();
}

Bounds3f Cylinder::local_bound() const 
{
    return
    {
        {-radius, min_height, -radius},
        { radius, max_height, radius}
    };
}

bool Cylinder::surface_intersect(const Ray3f& ray, hit_record& record, f32 cap_t) const
{
    const auto [ox, oy, oz] = ray.origin;
    const auto [dx, dy, dz] = ray.direction;

    const auto a = dx * dx + dz * dz;
    const auto b = 2 * (ox * dx + oz * dz);
    const auto c = ox * ox + oz * oz - radius * radius;

    const auto result = quadratic(a, b, c);
    if(!result) return false;

    auto [x1, x2] = result.value();
    if(x1 > x2) std::swap(x1, x2);

    // if surface not closed, should flip normal which inside the curve
    auto inside = false;
    // TODO: clean code
    Point3f p;
    auto t = x1;
    p = ray.at(t);
    auto phi = std::atan2(p.z, p.x);
    if(phi < 0) phi += 2 * PI<f32>;
    if(p.y < min_height || p.y > max_height || phi > max_angle)
    {
        t = x2;
        p = ray.at(t);
        phi = std::atan2(p.z, p.x);
        if(phi < 0) phi += 2 * PI<f32>;
        if(p.y < min_height || p.y > max_height || phi > max_angle)
            return false;
        else
            inside = true;
    }

    if(t >= cap_t) return false;

    const auto updated = record.set_t(t);
    if(updated)
    {
        record.n = get_normal(p.x, p.z);
        if(inside) record.n = -record.n;
        record.uv = get_uv(phi, p.y);
    }
    return updated;
}

bool Cylinder::check_cap(const Ray3f& ray, f32 t) const
{
    const auto x = ray.at<Axis::X>(t);
    const auto z = ray.at<Axis::Z>(t);
    return (x * x + z * z) <= radius * radius;
}

std::tuple<f32, f32> Cylinder::cap_intersect(const Ray3f& ray) const
{
    auto t = INFINITY_<f32>;
    f32 n;
    const auto intersect = [&](f32 cap_t, f32 y)
    {
        if(cap_t >= t || cap_t <= 0 || !check_cap(ray, cap_t))
            return;

        t = cap_t;
        // if surface not closed, the cap will be seen
        if(is_same_direction(Vector3f{0, y, 0}, ray.direction))
            y = -y;

        n = y;
    };

    if(cylinder_shape & cylinder_top)
        intersect(ray.inv_at<Axis::Y>(max_height), 1);

    if(cylinder_shape & cylinder_bottom)
        intersect(ray.inv_at<Axis::Y>(min_height), -1);

    return {t, n};
}

bool Cylinder::intersect(const Ray3f& ray3, hit_record& record) const 
{
    const auto ray = world_to_local * ray3;
    if(!cylinder_shape)
        return surface_intersect(ray, record, INFINITY_<f32>);

    const auto [t, n] = cap_intersect(ray);
    if(surface_intersect(ray, record, t)) return true;

    const auto updated = record.set_t(t);
    if(updated) record.n = {0, n, 0};
    return updated;
}

void Cylinder::get_intersect_record(const Ray3f& ray3, hit_record& record) const
{
    record.p = ray3.at(record.t_min);
    record.n = local_to_world * record.n;
}

f32 Cylinder::area() const
{
    return PI<f32> * 2 * radius * max_height;
}

std::tuple<Point3f, Vector3f, f32> Cylinder::sample(const Point2f& random) const
{
    const auto y = lerp(min_height, max_height, random.x);
    const auto theta  = random.y * max_angle;
    const auto [x, z] = Point2{radius * std::cos(theta), radius * std::sin(theta)};
    return
    {
        local_to_world * Point3{x, y, z},
        local_to_world * get_normal(x, z),
        pdf()
    };
}

f32 Cylinder::pdf() const
{
    return 1 / area();
}
