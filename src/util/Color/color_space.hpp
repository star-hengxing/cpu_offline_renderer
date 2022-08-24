#pragma once

#include <math/Vector3.hpp>
#include <util/marco.hpp>
#include "Color.hpp"

using Spectrum = Vector3f;

struct color_space
{
    Spectrum color;

    color_space(const Spectrum& color);
    
    color_space& tone_mapping();

    Color to_srgb() const;
};

NAMESPACE_BEGIN(RGB)

constexpr Spectrum purple = rgb_to_float<f32>({128,0,128});
constexpr Spectrum dodger_blue = rgb_to_float<f32>({30,144,255});
constexpr Spectrum pink = rgb_to_float<f32>({255,192,203});
constexpr Spectrum black = rgb_to_float<f32>(Color::black());
constexpr Spectrum red = rgb_to_float<f32>(Color::red());
constexpr Spectrum green = rgb_to_float<f32>(Color::green());
constexpr Spectrum blue = rgb_to_float<f32>(Color::blue());
constexpr Spectrum yellow = rgb_to_float<f32>(Color::yellow());
constexpr Spectrum magenta = rgb_to_float<f32>(Color::magenta());
constexpr Spectrum cyan = rgb_to_float<f32>(Color::cyan());
constexpr Spectrum white = rgb_to_float<f32>(Color::white());

NAMESPACE_END(RGB)
