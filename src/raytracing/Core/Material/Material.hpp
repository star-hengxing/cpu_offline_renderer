#pragma once

struct hit_record;

struct Material 
{
public:
    virtual ~Material() {}

    virtual void compute(hit_record& record) const = 0;
};
