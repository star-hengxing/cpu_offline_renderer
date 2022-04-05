#include <Core/Shape/Cylinder.hpp>

#include <Hinae/Transform.hpp>
#include <Hinae/equation.hpp>

#include <hit_record.hpp>

Cylinder::Cylinder(const Matrix4f& local_to_world, const Matrix4f& world_to_local
    , f32 height, f32 radius, f32 max_angle
    , max_shape cylinder_shape)
    : Shape(local_to_world, world_to_local)
    , height(height)
    , radius(radius), max_angle(max_angle)
    , cylinder_shape(cylinder_shape) {}

Bounds3f Cylinder::world_bound() const 
{
    return local_to_world * local_bound();
}

Bounds3f Cylinder::local_bound() const 
{
    return
    {
        {-radius, 0, -radius},
        { radius, height, radius}
    };
}

bool Cylinder::intersect(const Ray3f& ray3, hit_record& record) const 
{
    const auto ray = world_to_local * ray3;
    f32 a, b, c;
    const auto [ox, oy, oz] = ray.origin;
    const auto [dx, dy, dz] = ray.direction;

    a = dx * dx + dz * dz;
    b = 2 * (ox * dx + oz * dz);
    c = ox * ox + oz * oz - radius * radius;

    const auto result = quadratic(a, b, c);
    if(!result) return false;

    const auto [x1, x2] = result.value();

    f32 y, t = INFINITY_<f32>;
    y = ray.at<Axis::Y>(x1);
    if(y > 0 && y < height)
        t = x1;

    y = ray.at<Axis::Y>(x2);
    if(y > 0 && y < height)
        t = min(t, x2);

    if(cylinder_shape)
    {
        std::optional<f32> is_cap = check_cap(ray);
        if(is_cap) return record.set_t(is_cap.value());
    }

    const f32 x = ray.at<Axis::X>(t);
    const f32 z = ray.at<Axis::Z>(t);
    const f32 angle = std::abs(std::atan2(z, x));
    if(angle > max_angle) return false;

    return record.set_t(t);
}

void Cylinder::get_intersect_record(const Ray3f& ray3, hit_record& record) const
{
    record.p = ray3.at(record.t_min);
    const auto [x, y, z] = world_to_local * record.p;
    Vector3f normal{0, 0, 0};
    // TODO： 求交时候添加记录，这样求cap的法线能更robust
    if(y >= height - 0.001f)
        normal.y = 1;
    else if(y <= 0.001f)
        normal.y = -1;
    else
        normal = Vector3f{x, y, z}.normalized();
    record.n = local_to_world * normal;
}

bool Cylinder::check_cap(const Ray3f& ray, f32 t) const
{
    const f32 x = ray.at<Axis::X>(t);
    const f32 z = ray.at<Axis::Z>(t);
    return (x * x + z * z) <= radius * radius;
}

std::optional<f32> Cylinder::check_cap(const Ray3f& ray) const
{
    std::optional<f32> t;
    
    if(cylinder_shape & cylinder_top)
    {
        const f32 top_cap_t = ray.inv_at<Axis::Y>(height);
        if(check_cap(ray, top_cap_t))
            t = top_cap_t;
    }

    if(cylinder_shape & cylinder_bottom)
    {
        const f32 bottom_cap_t = -ray.origin.y / ray.direction.y;
        if(check_cap(ray, bottom_cap_t))
        {
            t = t.has_value() ? min(bottom_cap_t, t.value()) : bottom_cap_t;
        }
    }

    return t;
}
