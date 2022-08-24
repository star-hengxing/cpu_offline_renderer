#pragma once

template <typename A, typename B, typename... C>
constexpr auto max(A&& a, B&& b, C &&... c)
{
    if constexpr(sizeof...(c) == 0)
        return a > b ? a : b;
    else
        return max(max(a, b), c...);
}

template <typename... Args>
constexpr auto max_size()
{
    return max(sizeof(Args)...);
}

template <typename... T>
struct memory_pool
{
private:
    static constexpr auto size = max_size<T...>();

    char memory[size];

    memory_pool() {}

public:
    memory_pool(memory_pool&&) = delete;
    memory_pool(const memory_pool&) = delete;
    memory_pool& operator = (memory_pool&&) = delete;
    memory_pool& operator = (const memory_pool&) = delete;

    static auto& get() noexcept
    {
        static thread_local memory_pool pool;
        return pool;
    }

    template <typename U, typename... Args>
    U* alloc(Args&&... args)
    {
        return new (memory) U(std::forward<Args>(args)...);
    }
};

#include <raytracing/Core/BxDF/Lambertian.hpp>
#include <raytracing/Core/BxDF/Microfacet.hpp>
#include <raytracing/Core/BxDF/Dielectric.hpp>
#include <raytracing/Core/BxDF/Conductor.hpp>

using BxDF_memory_pool = memory_pool<Lambertian, Conductor, Dielectric, Microfacet>;
