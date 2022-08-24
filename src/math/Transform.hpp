#pragma once

#include "Quaternion.hpp"
#include "Bounds3.hpp"
#include "Matrix4.hpp"
#include "Vector3.hpp"
#include "Point3.hpp"
#include "Point4.hpp"
#include "Ray3.hpp"

template <arithmetic T>
constexpr Vector3<T> operator*(const Matrix4<T>& lhs, const Vector3<T>& rhs)
{
    const auto [x, y, z] = rhs;
    return
    {
        lhs[0][0] * x + lhs[0][1] * y + lhs[0][2] * z,
        lhs[1][0] * x + lhs[1][1] * y + lhs[1][2] * z,
        lhs[2][0] * x + lhs[2][1] * y + lhs[2][2] * z,
    };
}

template <arithmetic T>
constexpr Point4<T> operator*(const Matrix4<T>& lhs, const Point4<T>& rhs)
{
    const auto [x, y, z, w] = rhs;
    return
    {
        lhs[0][0] * x + lhs[0][1] * y + lhs[0][2] * z + lhs[0][3] * w,
        lhs[1][0] * x + lhs[1][1] * y + lhs[1][2] * z + lhs[1][3] * w,
        lhs[2][0] * x + lhs[2][1] * y + lhs[2][2] * z + lhs[2][3] * w,
        lhs[3][0] * x + lhs[3][1] * y + lhs[3][2] * z + lhs[3][3] * w
    };
}

template <arithmetic T>
constexpr Point3<T> operator*(const Matrix4<T>& lhs, const Point3<T>& rhs)
{
    const Point4<T> p = lhs * Point4{rhs};
    const T inv_w = reciprocal(p.w);
    return {p.x * inv_w, p.y * inv_w, p.z * inv_w};
}

template <arithmetic T>
constexpr Ray3<T> operator*(const Matrix4<T>& lhs, const Ray3<T>& rhs)
{
    return
    {
        lhs * rhs.origin,
        lhs * rhs.direction
    };
}

template <arithmetic T>
constexpr Bounds3<T> operator*(const Matrix4<T>& lhs, const Bounds3<T>& rhs)
{
    const Vector3<T> xa        = lhs.column(0) * rhs.p_min.x;
    const Vector3<T> xb        = lhs.column(0) * rhs.p_max.x;
    const Vector3<T> ya        = lhs.column(1) * rhs.p_min.y;
    const Vector3<T> yb        = lhs.column(1) * rhs.p_max.y;
    const Vector3<T> za        = lhs.column(2) * rhs.p_min.z;
    const Vector3<T> zb        = lhs.column(2) * rhs.p_max.z;
    const auto       translate = as<Point3, T>(lhs.column(3));
    return
    {
        translate + (min(xa, xb) + min(ya, yb) + min(za, zb)),
        translate + (max(xa, xb) + max(ya, yb) + max(za, zb))
    };
}

template <arithmetic T>
struct Transform
{
    static constexpr Matrix4<T> scale(T value)
    {
        return scale(value, value, value);
    }

    static constexpr Matrix4<T> scale(T x, T y, T z)
    {
        return
        {
            x,       ZERO<T>, ZERO<T>, ZERO<T>,
            ZERO<T>, y,       ZERO<T>, ZERO<T>,
            ZERO<T>, ZERO<T>, z,       ZERO<T>,
            ZERO<T>, ZERO<T>, ZERO<T>, ONE<T>
        };
    }

    static constexpr Matrix4<T> translate(const Vector3<T>& v)
    {
        return
        {
            ONE<T>,  ZERO<T>, ZERO<T>, v.x,
            ZERO<T>, ONE<T>,  ZERO<T>, v.y,
            ZERO<T>, ZERO<T>, ONE<T>,  v.z,
            ZERO<T>, ZERO<T>, ZERO<T>, ONE<T>
        };
    }

    static constexpr Matrix4<T> rotate(const Quaternion<T>& q)
    {
        const T a            = q.real;
        const auto [b, c, d] = q.image;
        return
        {
            1 - 2 * c * c - 2 * d * d, 2 * b * c - 2 * a * d, 2 * a * c + 2 * b * d, ZERO<T>,
            2 * b * c + 2 * a * d, 1 - 2 * b * b - 2 * d * d, 2 * c * d + 2 * a * b, ZERO<T>,
            2 * b * d - 2 * a * c, 2 * a * b - 2 * c * d, 1 - 2 * b * b - 2 * c * c, ZERO<T>,
            ZERO<T>, ZERO<T>, ZERO<T>, ONE<T>
        };
    }

    template <Axis axis>
    static Matrix4<T> rotate(T degree)
    {
        const T radian = to_radian(degree);
        const T s      = std::sin(radian);
        const T c      = std::cos(radian);
        if constexpr (axis == Axis::X)
        {
            return
            {
                ONE<T>,  ZERO<T>, ZERO<T>, ZERO<T>,
                ZERO<T>, c,       -s,      ZERO<T>,
                ZERO<T>, s,       c,       ZERO<T>,
                ZERO<T>, ZERO<T>, ZERO<T>, ONE<T>
            };
        }
        else if constexpr (axis == Axis::Y)
        {
            return
            {
                c,       ZERO<T>, s,       ZERO<T>,
                ZERO<T>, ONE<T>,  ZERO<T>, ZERO<T>,
                -s,      ZERO<T>, c,       ZERO<T>,
                ZERO<T>, ZERO<T>, ZERO<T>, ONE<T>
            };
        }
        else if constexpr (axis == Axis::Z)
        {
            return
            {
                c,      -s,       ZERO<T>, ZERO<T>,
                s,       c,       ZERO<T>, ZERO<T>,
                ZERO<T>, ZERO<T>, ONE<T>,  ZERO<T>,
                ZERO<T>, ZERO<T>, ZERO<T>, ONE<T>
            };
        }
    }

    static constexpr Matrix4<T> look_at(const Point3<T>& pos, const Point3<T>& at, const Vector3<T>& up)
    {
        const Vector3<T> forward = (at - pos).normalized();
        const Vector3<T> right   = cross(forward, up).normalized();
        const Vector3<T> Up      = cross(right, forward);
        const auto       eye     = as<Vector3, T>(pos);
        return
        {
            right.x, Up.x,   -forward.x, -dot(right, eye),
            right.y, Up.y,   -forward.y, -dot(Up, eye),
            right.z, Up.z,   -forward.z,  dot(forward, eye),
            ZERO<T>, ZERO<T>, ZERO<T>,    ONE<T>
        };
    }

    static constexpr Matrix4<T> orthographic(T width, T height, T z_near, T z_far)
    {
        const T a11 = 2 / width;
        const T a22 = 2 / height;
        const T a32 = 2 / (z_near - z_far);
        const T a33 = (z_far + z_near) / (z_near - z_far);
        return
        {
            a11,     ZERO<T>, ZERO<T>, ZERO<T>,
            ZERO<T>, a22,     ZERO<T>, ZERO<T>,
            ZERO<T>, ZERO<T>, a32,     a33,
            ZERO<T>, ZERO<T>, ZERO<T>, ONE<T>
        };
    }

    static Matrix4<T> perspective(T fov, T aspect, T z_near, T z_far)
    {
        const T tan = std::tan(to_radian(fov / 2));
        const T cot = 1 / tan;

        const T a11 = cot / aspect;
        const T a22 = cot;
        const T a32 = (z_far + z_near) / (z_near - z_far);
        const T a33 = (static_cast<T>(2) * z_far * z_near) / (z_far - z_near);
        return
        {
            a11,     ZERO<T>,  ZERO<T>, ZERO<T>,
            ZERO<T>, a22,      ZERO<T>, ZERO<T>,
            ZERO<T>, ZERO<T>,  a32,     a33,
            ZERO<T>, ZERO<T>, -ONE<T>,  ZERO<T>
        };
    }
};
