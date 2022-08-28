#pragma once

#include <raytracing/hit_record.hpp>
#include <util/basic_type.hpp>

#include "Texture.hpp"

struct checker_texture : public Texture
{
private:
    usize    width, height;
    Spectrum albedo_u, albedo_v;

public:
    checker_texture(usize width, usize height, const Spectrum& albedo_u, const Spectrum& albedo_v) :
        width(width), height(height), albedo_u(albedo_u), albedo_v(albedo_v) {}

    virtual Spectrum evaluate(const hit_record& record) const override
    {
        const auto u = static_cast<usize>(std::floor(record.uv.x * width));
        const auto v = static_cast<usize>(std::floor(record.uv.y * height));
        return (u + v) % 2 == 0 ? albedo_u : albedo_v;
    }
};
