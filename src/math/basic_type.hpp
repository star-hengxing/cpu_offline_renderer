#pragma once

#include <cstdint>
#include <concepts>
#include <numbers>
#include <limits>

#include <util/basic_type.hpp>

template <typename T>
concept arithmetic = std::is_arithmetic_v<T>;

template <typename T>
concept signed_numeric = std::is_signed_v<T>;

template <arithmetic T>
inline constexpr T ZERO = static_cast<T>(0);

template <arithmetic T>
inline constexpr T ONE = static_cast<T>(1);

template <arithmetic T>
inline constexpr T MAX_NUMBER = std::numeric_limits<T>::max();

template <arithmetic T>
inline constexpr T MIN_NUMBER = std::numeric_limits<T>::min();

template <std::floating_point T>
inline constexpr T INFINITY_ = std::numeric_limits<T>::infinity();

template <typename T>
inline constexpr T PI = std::numbers::pi_v<T>;

template <typename T>
inline constexpr T INV_PI = std::numbers::inv_pi_v<T>;

template <std::floating_point T>
inline constexpr T INV_2PI = static_cast<T>(0.15915494309189533577);

template <std::floating_point T>
inline constexpr T INV_4PI = static_cast<T>(0.07957747154594766788);

template <std::floating_point T>
inline constexpr T PI_OVER_2 = static_cast<T>(1.57079632679489661923);

template <std::floating_point T>
inline constexpr T PI_OVER_4 = static_cast<T>(0.78539816339744830961);

template <typename T>
inline constexpr T SQRT2 = std::numbers::sqrt2_v<T>;

template <arithmetic T>
struct Vector3;

template <arithmetic T>
struct Vector2;

template <arithmetic T>
struct Point4;

template <arithmetic T>
struct Point3;

template <arithmetic T>
struct Point2;

template <arithmetic T>
struct Matrix4;

template <arithmetic T>
struct Ray3;

template <arithmetic T>
struct Bounds3;

template <arithmetic T>
struct Quaternion;

template <arithmetic T, u32 a, u32 c, u32 m>
struct Linear_congruential_generator;

enum class Axis : usize { X = 0, Y = 1, Z = 2 };
