#pragma once

#include <Hinae/Point2.hpp>
#include <Hinae/rng.hpp>

using namespace Hinae;

struct Sampler
{
private:
    RNG<f32> rng;

public:
    Point2f get_2D();
};
