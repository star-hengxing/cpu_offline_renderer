#pragma once

#include "Vector3.hpp"

using Quaternionf = Quaternion<f32>;
using Quaterniond = Quaternion<f64>;
using Quaternioni = Quaternion<isize>;

template <arithmetic T>
struct Quaternion
{
    T real;
    Vector3<T> image;

    constexpr Quaternion(T real, const Vector3<T>& v)
        : real(real), image(v) {}
    constexpr Quaternion(T w, T x, T y, T z) : Quaternion(w, {x, y, z}) {}

    constexpr Quaternion() = default;
    constexpr auto operator <=> (const Quaternion<T>&) const = default;

    constexpr Quaternion<T> operator + (T v) const { return {real + v, image + v}; }

    constexpr Quaternion<T> operator * (T v) const { return {real * v, image * v}; }

    constexpr Quaternion<T> operator * (const Quaternion<T>& q) const
    {
        const T a = real;
        const auto [b, c, d] = image;
        const T e = q.real;
        const auto [f, g, h] = q.image;
        return
        {
            a * e - b * f - c * g - d * h,
            b * e + a * f - d * g + c * h,
            c * e + d * f + a * g - b * h,
            d * e - c * f + b * g + a * h
        };
    }

    static constexpr Quaternion<T> pure(const Vector3<T>& v) { return {ZERO<T>, v}; }

    static Quaternion<T> rotate(T angle, const Vector3<T>& v)
    {
        const T a = angle / 2;
        return {std::cos(a), v * std::sin(a)};
    }

    constexpr Quaternion<T> conjugate() const { return {real, -image}; }

    constexpr T norm2() const { return real * real + image.norm2(); }

    T norm() const { return std::sqrt(norm2()); }

    Quaternion<T> normalized() const { return (*this) * reciprocal(norm()); }

    constexpr Quaternion<T> inverse() const { return conjugate() * reciprocal(norm2()); }
};

template <arithmetic T>
constexpr Quaternion<T> operator + (T v, const Quaternion<T>& q) { return q + v; }

template <arithmetic T>
constexpr Quaternion<T> operator * (T v, const Quaternion<T>& q) { return q * v; }

template <arithmetic T>
std::ostream& operator << (std::ostream& os, const Quaternion<T>& q)
{
    return os << std::make_tuple(q.real, q.image);
}
