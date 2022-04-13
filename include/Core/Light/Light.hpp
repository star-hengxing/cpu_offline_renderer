#pragma once

#include <Hinae/Vector3.hpp>
#include <Hinae/Point3.hpp>
#include <Hinae/Point2.hpp>

using namespace Hinae;

using Spectrum = Hinae::Vector3f;

struct hit_record;

struct Light
{
public:
    enum Flags
    {
        delta_position  = 0b0001,
        delta_direction = 0b0010,
        area            = 0b0100,
        infinite        = 0b1000
    };

    Flags flag;

public:
    Light(Flags flag);

    virtual ~Light() = default;

    virtual Spectrum Li(const hit_record& record, const Vector3f& w) const = 0;

    virtual std::tuple<Vector3f, f32> get_dir_and_distance(const Point3f& p) const = 0;
    // dir, distance, pdf
    virtual std::tuple<Vector3f, f32, f32> sample_li(const Point3f& p, const Point2f& random) const = 0;

    virtual f32 pdf_li(const Point3f& p) const = 0;
};
