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
    static constexpr auto block = 2;
    static constexpr auto size = max_size<T...>();

    usize index = 0;
    char memory[size * block];

public:
    memory_pool() {}
    memory_pool(memory_pool&&) = delete;
    memory_pool(const memory_pool&) = delete;

    static auto& get() noexcept
    {
        static thread_local memory_pool pool;
        return pool;
    }

    template <typename U, typename... Args>
    U* alloc(Args&&... args)
    {
        index = (index < block) ? index + 1 : 0;
        void* address = std::addressof(memory[size * (index - 1)]);
        return new (address) U(std::forward<Args>(args)...);
    }
};

#include <Core/BxDF/Lambertian.hpp>
#include <Core/BxDF/Specular.hpp>

using BxDF_memory_pool = memory_pool<Lambertian, Specular>;
