#pragma once

#include "basic.hpp"

using Vector2f = Vector2<f32>;
using Vector2d = Vector2<f64>;
using Vector2i = Vector2<isize>;

template <arithmetic T>
struct Vector2
{
    T x, y;
    
    constexpr Vector2(T v) : x(v), y(v) {}
    constexpr Vector2(T x, T y) : x(x), y(y) {}

    template <arithmetic U>
    constexpr explicit Vector2(const Vector2<U>& p)
        : x(static_cast<T>(p.x))
        , y(static_cast<T>(p.y)) {}

    constexpr Vector2() = default;
    constexpr auto operator <=> (const Vector2<T>&) const = default;

    constexpr Vector2<T> operator - () const { return {-x, -y}; }

    constexpr Vector2<T> operator + (T rhs) const { return {x + rhs, y + rhs}; }
    constexpr Vector2<T> operator - (T rhs) const { return {x - rhs, y - rhs}; }
    constexpr Vector2<T> operator * (T rhs) const { return {x * rhs, y * rhs}; }
    constexpr Vector2<T> operator / (T rhs) const { return (*this) * reciprocal(rhs); }

    constexpr Vector2<T> operator + (const Vector2<T>& rhs) const { return {x + rhs.x, y + rhs.y}; }
    constexpr Vector2<T> operator - (const Vector2<T>& rhs) const { return {x - rhs.x, y - rhs.y}; }
    constexpr Vector2<T> operator * (const Vector2<T>& rhs) const { return {x * rhs.x, y * rhs.y}; }
    constexpr Vector2<T> operator / (const Vector2<T>& rhs) const { return {x / rhs.x, y / rhs.y}; }

    constexpr void operator += (T rhs) { x += rhs; y += rhs; }
    constexpr void operator -= (T rhs) { x -= rhs; y -= rhs; }
    constexpr void operator *= (T rhs) { x *= rhs; y *= rhs; }
    constexpr void operator /= (T rhs) { (*this) *= reciprocal(rhs); }

    constexpr void operator += (const Vector2<T>& rhs) { x += rhs.x; y += rhs.y; }
    constexpr void operator -= (const Vector2<T>& rhs) { x -= rhs.x; y -= rhs.y; }
    constexpr void operator *= (const Vector2<T>& rhs) { x *= rhs.x; y *= rhs.y; }
    constexpr void operator /= (const Vector2<T>& rhs) { x /= rhs.x; y /= rhs.y; }

    constexpr T norm2() const { return x * x + y * y; }
    constexpr T norm()  const { return static_cast<T>(std::sqrt(norm2())); }

    constexpr void normalize() { (*this) *= reciprocal(norm()); }
    constexpr Vector2<T> normalized() const { return (*this) * reciprocal(norm()); }

    constexpr T max_component() const { return max(x, y); }

    constexpr T min_component() const { return min(x, y); }

    constexpr Axis max_dimension() const { return (x > y ? Axis::X : Axis::Y); }

    constexpr Axis min_dimension() const { return (x < y ? Axis::X : Axis::Y); }

    T operator [] (usize i) const
    { 
        assert(i <= 1);
        return (&x)[i];
    }
    
    T& operator [] (usize i)
    { 
        assert(i <= 1);
        return (&x)[i];
    }

    T operator [] (Axis axis) const
    { 
        return (&x)[static_cast<usize>(axis)];
    }

    T& operator [] (Axis axis)
    { 
        return (&x)[static_cast<usize>(axis)];
    }
};

template <arithmetic T>
constexpr T dot(const Vector2<T>& lhs, const Vector2<T>& rhs)
{
    return lhs.x * rhs.x + lhs.y * rhs.y;
}

template <arithmetic T>
constexpr T cross(const Vector2<T>& lhs, const Vector2<T>& rhs)
{
    return lhs.x * rhs.y - lhs.y * rhs.x;
}

template <arithmetic T>
constexpr Vector2<T> operator + (T lhs, const Vector2<T>& rhs) { return rhs + lhs; }
template <arithmetic T>
constexpr Vector2<T> operator * (T lhs, const Vector2<T>& rhs) { return rhs * lhs; }

template <arithmetic T>
std::ostream& operator << (std::ostream& os, const Vector2<T>& v)
{
    return os << std::make_tuple(v.x, v.y);
}
