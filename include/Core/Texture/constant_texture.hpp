#pragma once

#include "Texture.hpp"

#include <memory>

template <typename T>
struct constant_texture : public Texture<T>
{
private:
    T value;

public:
    constant_texture(const T& value) : value(value) {}

    static std::shared_ptr<Texture<T>> make(const T& value)
    {
        return std::make_shared<constant_texture<T>>(value);
    }

    virtual T evaluate(const hit_record& record) const
    {
        return value;
    }
};
