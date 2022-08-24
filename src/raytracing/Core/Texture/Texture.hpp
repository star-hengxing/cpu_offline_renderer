#pragma once

struct hit_record;

template <typename T>
struct Texture
{
public:
    virtual ~Texture() = default;

    virtual T evaluate(const hit_record& record) const = 0;
};
