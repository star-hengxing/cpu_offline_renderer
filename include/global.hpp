#pragma once

#include <Hinae/Vector3.hpp>
#include <Hinae/Point3.hpp>

using namespace Hinae;

#include <bit>

template <arithmetic T>
bool is_same_direction(const Vector3<T>& v1, const Vector3<T>& v2)
{
    return dot(v1, v2) > 0;
}

template <arithmetic T>
Vector3<T> flip_normal(const Vector3<T>& v, const Vector3<T>& n)
{
    return is_same_direction(v, n) ? n : -n;
}
// Avoiding Self-Intersection
struct Offset
{
private:
    static constexpr f32 scale   = 256.0f;
    static constexpr f32 origin  = 1.0f / 32.0f;
    static constexpr f32 epsilon = 1.0f / 65536.0f;

public:

    static constexpr Point3f native(const Point3f& point, const Vector3f& normal)
    {
        return point + normal * epsilon;
    }
    // come from Ray Tracing Gems
    static constexpr Point3f robust(const Point3f& point, const Vector3f& normal)
    {
        const auto& p = point;
        const auto& n = normal;
        // Scaling is multiplicative, translation is additive
        const auto off_isize = static_cast<Vector3<int>>(n * scale);

        const Point3 point_isize
        {
            std::bit_cast<f32>(std::bit_cast<int>(p.x) + ((p.x < 0) ? -off_isize.x : off_isize.x)),
            std::bit_cast<f32>(std::bit_cast<int>(p.y) + ((p.y < 0) ? -off_isize.y : off_isize.y)),
            std::bit_cast<f32>(std::bit_cast<int>(p.z) + ((p.z < 0) ? -off_isize.z : off_isize.z))
        };

        return
        {
            std::abs(p.x) < origin ? p.x + n.x * epsilon : point_isize.x,
            std::abs(p.y) < origin ? p.y + n.y * epsilon : point_isize.y,
            std::abs(p.z) < origin ? p.z + n.z * epsilon : point_isize.z
        };
    }
};
