#include "util.hpp"

Timer::Timer() : t0(steady_clock::now()) {}

template <typename T>
void Timer::display(T number, const std::string_view& unit)
{
    std::cout << "time elapsed: " << number << unit.data() << '\n';
}

void Timer::elapsed() const
{
    auto p = steady_clock::now() - t0;
    f64 ms = duration_cast<f64_ms>(p).count();
    if(ms < 1000)
        display(ms, "ms");
    else
    {
        display(static_cast<usize>(ms / 1000), "s");
    }
}
