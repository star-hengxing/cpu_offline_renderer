#pragma once

#include <iostream>
#include <chrono>

#include "basic_type.hpp"

struct Timer
{
private:
    using f64_ms = std::chrono::duration<f64, std::milli>;
    std::chrono::steady_clock::time_point t0;

public:
    Timer() : t0(std::chrono::steady_clock::now()) {}

    template <typename T>
    static void display(T number, const std::string_view& unit)
    {
        std::cout << "time elapsed: " << number << unit.data() << '\n';
    }

    void elapsed() const
    {
        auto p = std::chrono::steady_clock::now() - t0;
        f64 ms = std::chrono::duration_cast<f64_ms>(p).count();
        if(ms < 1000)
        {
            display(ms, "ms");
        }
        else
        {
            display(static_cast<usize>(ms / 1000), "s");
        }
    }

    template <typename T, typename... Args>
    static auto elapsed(T fn, Args&&... args)
    {
        Timer timer;
        if constexpr (std::is_same_v<decltype(fn(std::forward<Args>(args)...)), void>)
        {
            fn(std::forward<Args>(args)...);
            timer.elapsed();
        }
        else
        {
            auto ret = fn(std::forward<Args>(args)...);
            timer.elapsed();
            return ret;
        }
    }

    template <typename T, typename U, typename... Args>
    static U elapsed(T* this_ptr, U (T::*member_fn)(Args...), Args&&... args)
    {
        Timer timer;
        if constexpr (std::is_same_v<U, void>)
        {
            (this_ptr->*member_fn)(std::forward<Args>(args)...);
            timer.elapsed();
        }
        else
        {
            U ret = (this_ptr->*member_fn)(std::forward<Args>(args)...);
            timer.elapsed();
            return ret;
        }
    }
};
