#pragma once

#include <source_location>
#include <iostream>
#include <ranges>

#include "range.hpp"

#ifdef NDEBUG
#define dbg(expr) ((void)0)
#else
#define dbg(expr) std::cout<< '[' << std::source_location::current().file_name() << ':'<< std::source_location::current().line() << "] " << #expr << " = " << expr << '\n';
#endif // NDEBUG

[[noreturn]] inline void unimplemented(
    const std::source_location location = std::source_location::current())
{
    std::cout << '\n'
              << '['  << location.file_name()
              << ':'  << location.line()
              << "] " << location.function_name()
              << " unimplemented!\n";
    exit(-1);
}

template <typename T>
void for_each(usize size, T fn)
{
    if constexpr(std::is_void_v<decltype(fn())>)
    {
        for ([[maybe_unused]] auto _ : range(size))
        {
            fn();
        }
    }
}

template <typename T>
concept not_void = !std::is_void_v<T>;

template <typename T> requires
requires(T fn) { { fn() } -> not_void; }
constexpr auto arithmetic_mean(usize size, T fn)
{
    decltype(fn()) sum = 0;
    for ([[maybe_unused]] auto _ : range(size))
    {
        sum += fn();
    }
    return sum / size;
}
