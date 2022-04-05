#include <Core/Shape/Cone.hpp>

#include <Hinae/Transform.hpp>
#include <Hinae/equation.hpp>

#include <hit_record.hpp>

Cone::Cone(const Matrix4f& local_to_world, const Matrix4f& world_to_local
    , f32 radius, f32 height, f32 max_angle)
    : Shape(local_to_world, world_to_local)
    , radius(radius), height(height)
    , max_angle(max_angle) {}

Bounds3f Cone::world_bound() const 
{
    return local_to_world * local_bound();
}

Bounds3f Cone::local_bound() const 
{
    return
    {
        {-radius, 0, -radius},
        { radius, height, radius}
    };
}

bool Cone::intersect(const Ray3f& ray3, hit_record& record) const 
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
    
    f32 h, t = INFINITY_<f32>;
    h = ray.at<Axis::Y>(x1);
    if(h > 0 && h < height)
        t = x1;

    h = ray.at<Axis::Y>(x2);
    if(h > 0 && h < height)
        t = min(t, x2);

    const Point3f p = ray.at(t);
    const auto [x, y, z] = p;
    const Point3f H{0, height, 0};
    const Vector3f V = -H;
    const Vector3f tmp{radius, -1, 0};
    f32 current = std::abs(dot((p - H).normalized(), V));
    f32 cos = std::abs(dot(tmp, V));
    if(current > cos) return false;
    
    const f32 angle = std::abs(std::atan2(z, x));
    if(angle > max_angle) return false;

    return record.set_t(t);
}

void Cone::get_intersect_record(const Ray3f& ray3, hit_record& record) const
{
    record.p = ray3.at(record.t_min);
    const auto [x, y, z] = world_to_local * record.p;
    record.n = local_to_world * Vector3f{x, y, z}.normalized();
}
