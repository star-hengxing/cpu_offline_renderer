#include "color_space.hpp"

constexpr Spectrum ACES(const Spectrum& color)
{
    constexpr f32 A = 2.51f;
    constexpr f32 B = 0.03f;
    constexpr f32 C = 2.43f;
    constexpr f32 D = 0.59f;
    constexpr f32 E = 0.14f;
    return (color * (A * color + B)) / (color * (C * color + D) + E);
}

template <std::floating_point T>
constexpr T linear_to_srgb(T x)
{
    if(x <= static_cast<T>(0.0031308))
        return x * static_cast<T>(12.92);
    else
        return std::pow(x * static_cast<T>(1.055), ONE<T> / static_cast<T>(2.4)) - static_cast<T>(0.055);
}

color_space::color_space(const Spectrum& color) : color(color) {}

color_space& color_space::tone_mapping()
{
    color = ACES(color);
    return *this;
}

Color color_space::to_srgb() const
{
    Spectrum ret;
    for(usize i = 0; i < 3; i++)
        ret[i] = linear_to_srgb(color[i]);
    return float_to_rgb(ret);
}
