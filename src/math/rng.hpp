#pragma once

#include <random>
#include <ctime>

#include "basic_type.hpp"

template <arithmetic T>
using LCG = Linear_congruential_generator<T, 16807u, 0u, MAX_NUMBER<u32>>;

template <arithmetic T, u32 A, u32 C, u32 M>
struct Linear_congruential_generator
{
public:
    Linear_congruential_generator(u32 seed = std::time(nullptr)) : seed(seed) {}

    constexpr T get()
    {
        seed = (multiplier * seed) % modulus;
        if constexpr (std::is_floating_point_v<T>)
            return static_cast<T>(seed) / static_cast<T>(modulus);
        else
            return static_cast<T>(seed);
    }

private:
    static constexpr u32 multiplier = A;
    static constexpr u32 increment  = C;
    static constexpr u32 modulus    = M;

    u32 seed;
};

template <std::floating_point T>
struct RNG
{
private:
    std::mt19937                      rng;
    std::uniform_real_distribution<T> dist;

public:
    RNG(u32 seed) : rng(seed) {}

    RNG() : RNG(std::random_device{}()) {}

    RNG(const RNG<T>&)               = delete;
    RNG<T>& operator=(const RNG<T>&) = delete;

    T get()
    {
        return dist(rng);
    }

    RNG<T> clone() const noexcept
    {
        RNG<T> ret;
        return ret;
    }
};
