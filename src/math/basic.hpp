#pragma once

#include <cassert>
#include <cmath>

#include <iostream>
#include <tuple>

#include "basic_type.hpp"

template <arithmetic T>
constexpr bool is_zero(T x) { return x == ZERO<T>; }

template <arithmetic T>
constexpr bool is_one(T x) { return x == ONE<T>; }

template <std::integral T>
constexpr bool is_odd(T x) { return x % 2 != 0; }

template <std::integral T>
constexpr bool is_even(T x) { return x % 2 == 0; }

template <std::signed_integral T>
constexpr T abs(T x) { return x >= ZERO<T> ? x : -x; }

template <arithmetic T>
constexpr T clamp(T low, T value, T high)
{
    if(value <= low)       return low;
    else if(value >= high) return high;
    else                   return value;
}

template <signed_numeric T>
constexpr T sign(T x)
{
    if(x > ZERO<T>)      return ONE<T>;
    else if(x < ZERO<T>) return -ONE<T>;
    else                 return ZERO<T>;
}

template <arithmetic T>
constexpr T reciprocal(T x)
{
    assert(!is_zero(x));
    return ONE<T> / x;
}

template <std::floating_point T>
constexpr T to_degree(T radian) { return (static_cast<T>(180) / PI<T>) * radian; }

template <std::floating_point T>
constexpr T to_radian(T degree) { return (PI<T> / static_cast<T>(180)) * degree; }

template <typename T>
constexpr T pow2(T x)
{
    return x * x;
}

template <typename T>
constexpr T pow3(T x)
{
    return x * x * x;
}

template <typename T>
constexpr T pow4(T x)
{
    const T x2 = pow2(x);
    return x2 * x2;
}

template <typename T>
constexpr T pow5(T x)
{
    const T x2 = pow2(x);
    return x2 * x2 * x;
}

template <arithmetic T>
constexpr T max(T x, T y)
{
    return (x > y ? x : y);
}

template <arithmetic T>
constexpr T min(T x, T y)
{
    return (x < y ? x : y);
}

template <arithmetic T>
constexpr T max(T x, T y, T z)
{
    return max(x, max(y, z));
}

template <arithmetic T>
constexpr T min(T x, T y, T z)
{
    return min(x, min(y, z));
}

template <template <arithmetic> typename T, arithmetic U>
constexpr T<U> max(const T<U>& g1, const T<U>& g2)
{
    if constexpr(sizeof(T<U>) / sizeof(U) == 2)
    {
        return {max(g1.x, g2.x), max(g1.y, g2.y)};
    }
    else
    {
        static_assert(sizeof(T<U>) / sizeof(U) ==  3);
        return {max(g1.x, g2.x), max(g1.y, g2.y), max(g1.z, g2.z)};
    }
}

template <template <arithmetic> typename T, arithmetic U>
constexpr T<U> min(const T<U>& g1, const T<U>& g2)
{
    if constexpr(sizeof(T<U>) / sizeof(U) == 2)
    {
        return {min(g1.x, g2.x), min(g1.y, g2.y)};
    }
    else
    {
        static_assert(sizeof(T<U>) / sizeof(U) ==  3);
        return {min(g1.x, g2.x), min(g1.y, g2.y), min(g1.z, g2.z)};
    }
}

template <template <arithmetic> typename T, arithmetic U>
constexpr T<U> clamp(U low, const T<U>& x, U high)
{
    T<U> ret;
    for(usize i = 0; i < sizeof(T<U>) / sizeof(U); i++)
        ret[i] = clamp(low, x[i], high);
    return ret;
}

template <template <arithmetic> typename T, arithmetic U>
constexpr T<U> abs(const T<U>& x)
{
    T<U> ret;
    for(usize i = 0; i < sizeof(T<U>) / sizeof(U); i++)
        ret[i] = std::abs(x[i]);
    return ret;
}

template <template <arithmetic> typename T, arithmetic U>
constexpr T<U> ceil(const T<U>& x)
{
    T<U> ret;
    for(usize i = 0; i < sizeof(T<U>) / sizeof(U); i++)
        ret[i] = std::ceil(x[i]);
    return ret;
}

template <template <arithmetic> typename T, arithmetic U>
constexpr T<U> floor(const T<U>& x)
{
    T<U> ret;
    for(usize i = 0; i < sizeof(T<U>) / sizeof(U); i++)
        ret[i] = std::floor(x[i]);
    return ret;
}

template <template <arithmetic> typename T, arithmetic U>
constexpr T<U> pow(const T<U>& x, U exp)
{
    T<U> ret;
    for(usize i = 0; i < sizeof(T<U>) / sizeof(U); i++)
        ret[i] = std::pow(x[i], exp);
    return ret;
}

template <template <arithmetic> typename T, arithmetic U>
constexpr T<U> sqrt(const T<U>& x, U exp)
{
    T<U> ret;
    for(usize i = 0; i < sizeof(T<U>) / sizeof(U); i++)
        ret[i] = std::sqrt(x[i], exp);
    return ret;
}

template <typename T, typename U>
constexpr auto lerp(const T& left, const T& right, const U& w)
{
    return left + (right - left) * w;
}

template
<
    template <arithmetic> typename To,   arithmetic U,
    template <arithmetic> typename From, arithmetic T
>
constexpr To<U> as(const From<T>& from)
{
    static_assert(sizeof(To<U>) / sizeof(U) <= sizeof(From<T>) / sizeof(T));
    To<U> to;
    for(usize i = 0; i < sizeof(To<U>) / sizeof(U); i++)
        to[i] = static_cast<U>(from[i]);
    return to;
}

template<typename... Ts>
std::ostream& operator << (std::ostream& os, const std::tuple<Ts...>& tuple)
{
    std::apply
    (
        [&os](const Ts&... args)
        {
            os << '(';
            std::size_t n{0};
            ((os << args << (++n != sizeof...(Ts) ? ", " : "")), ...);
            os << ')';
        }, tuple
    );
    return os;
}
