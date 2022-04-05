#pragma once

#include <Hinae/Vector3.hpp>
#include <Hinae/Point3.hpp>
#include <Hinae/Ray3.hpp>

using namespace Hinae;

using Spectrum = Hinae::Vector3f;

struct Light
{
public:
    virtual ~Light() = default;

    virtual Spectrum Li(const Point3f& p) const = 0;

    virtual std::tuple<Vector3f, f32> get_dir_and_distance(const Point3f& p) const = 0;
};
