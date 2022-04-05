#pragma once

#include "constant_texture.hpp"

#include <memory>
#include <cmath>

using usize = std::size_t;

template <typename T>
struct checker_texture : public Texture<T>
{
private:
    usize width, height;
    constant_texture<T> c1, c2;

public:
    checker_texture(usize width, usize height, const T& c1, const T& c2)
        : width(width), height(height)
        , c1(constant_texture<T>(c1))
        , c2(constant_texture<T>(c2)) {}

    static std::shared_ptr<Texture<T>> make(usize width, usize height, const T& c1, const T& c2)
    {
        return std::make_shared<checker_texture<T>>(width, height, c1, c2);
    }

    virtual T evaluate(const hit_record& record) const
    {
        const auto u = static_cast<usize>(std::floor(record.uv.x * width));
        const auto v = static_cast<usize>(std::floor(record.uv.y * height));
        return (u + v) % 2 == 0 ? c1.evaluate(record) : c2.evaluate(record);
    }
};
