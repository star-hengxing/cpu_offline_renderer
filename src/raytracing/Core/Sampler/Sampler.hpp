#pragma once

#include <math/Point2.hpp>
#include <math/rng.hpp>

struct Sampler
{
private:
    RNG<f32> rng;

public:
    Point2f get_2D();
};
