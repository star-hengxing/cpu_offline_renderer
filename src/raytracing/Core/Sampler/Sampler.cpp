#include <raytracing/Core/Sampler/Sampler.hpp>

Point2f Sampler::get_2D()
{
    return {rng.get(), rng.get()};
}
