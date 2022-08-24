#pragma once

#include <source_location>
#include <algorithm>
#include <iostream>
#include <chrono>
#include <ranges>

#ifdef NDEBUG
#define dbg(expr) ((void)0)
#else
#define dbg(expr) std::cout<< '[' << std::source_location::current().file_name() << ':'<< std::source_location::current().line() << "] " << #expr << " = " << expr << '\n';
#endif // NDEBUG

using usize = std::size_t;
using f64     = double;

#if defined(__clang__)
template <std::integral T>
struct range
{
private:
    T start, stop, step;

private:
    struct range_iterator
    {
    private:
        T value, step;

    public:
        constexpr range_iterator(T value, T step) : value(value), step(step) {}
        T& operator * () { return value; }
        range_iterator& operator ++ () { value += step; return *this; }
        bool operator != (const range_iterator& other) { return value < other.value; }
    };

public:
    constexpr range(T start, T stop, T step = 1) : start(start), stop(stop), step(step) {}
    constexpr range(T stop) : range(0, stop) {}
    constexpr range_iterator begin() { return range_iterator(start, step); }
    constexpr range_iterator end() { return range_iterator(stop, step); }

    range() = delete;
};

template<typename T>
struct Enumerate
{
private:
    T it;
    usize index;
    const usize step;
    decltype(std::begin(it)) begin_;
    const decltype(std::end(it)) end_;

public:
    Enumerate(T it, usize step) : it(it), index(0), step(step),
        begin_(std::begin(it)), end_(std::end(it)) {}

    const Enumerate& begin() const { return *this; }
    const Enumerate& end()   const { return *this; }

    bool operator != (const Enumerate&) const
    {
        return begin_ != end_;
    }

    void operator ++ ()
    {
        begin_ += 1;
        index  += step;
    }

    auto operator * () const -> std::pair<usize, decltype(*begin_)>
    {
        return {index, *begin_};
    }
};

template<std::ranges::range T>
auto enumerate(T& it, usize step = 1) -> Enumerate<T>
{
    return {std::forward<T>(it), step};
}

template <typename T>
struct split_iterator
{
private:
    const std::string_view& sv;
    const T& delim;
    usize start_, end_;

public:
    split_iterator(const std::string_view& sv, const T& delim)
        : sv(sv), delim(delim)
        , start_(find_first_not(0))
        , end_(sv.find(delim, start_)) {}

    const split_iterator& begin() const { return *this; }
    const split_iterator& end()   const { return *this; }

    auto find_first_not(usize pos = 0) -> usize
    {
        for(usize i = pos; i < sv.size(); i += 1)
        {
            if (sv[i] != delim) return i;
        }
        return std::string_view::npos;
    };   

    bool operator != (const split_iterator&)
    {
        return end_ != std::string_view::npos
            && start_ != std::string_view::npos;
    }

    void operator ++ ()
    {
        start_ = find_first_not(end_);
        end_ = sv.find(delim, start_);
        if(end_ == std::string_view::npos)
        {
            end_ = sv.size();
        }
    }

    auto operator * () const -> std::string_view
    {
        return {&sv[start_], end_ - start_};
    }
};

template <typename T>
constexpr auto split(const std::string_view& sv, const T& delim)
{
    return split_iterator{sv, delim};
}
#else
template <std::integral T>
constexpr auto range(T end)
{
    return std::views::iota(static_cast<T>(0), end);
}

template <std::integral T>
constexpr auto range(T start, T end)
{
    return std::views::iota(start, end);
}

template <typename T>
struct enumerated_element
{
    usize index;
    T& element;
};

template <std::ranges::range T>
constexpr auto enumerate(T& range, usize step = 1)
{
    return range | std::views::transform(
        [i = usize{}, step](auto& element) mutable
        {
            i += step;
            return enumerated_element{i - step, element};
        }
    );
}

template <typename T>
constexpr auto split(const std::string_view& sv, const T& delim)
{
    return sv
        | std::ranges::views::split(delim)
        | std::ranges::views::transform([](auto&& rng)
    {
        return std::string_view(std::addressof(*rng.begin()), std::ranges::distance(rng));
    });
}

#endif // defined(__clang__)

template <std::ranges::range Container1, std::ranges::range Container2, std::copy_constructible Function>
void map(const Container1& src, Container2& dst, Function f)
{
    std::ranges::transform(src.begin(), src.end(), dst.begin(), f);
    // std::ranges::copy(std::views::transform(src, f), dst.begin());
}

[[noreturn]] inline void unimplemented(const std::source_location location = std::source_location::current())
{
    std::cout << '\n'
              << '['  << location.file_name()
              << ':'  << location.line()
              << "] " << location.function_name()
              << " unimplemented!\n";
    exit(-1);
}

template <typename... Args>
void println(const Args&... args)
{
    ((std::cout << args << '\n'), ...);
}

template <typename... Args>
void print(const Args&... args)
{
    ((std::cout << args << ' '), ...);
}

template <typename T>
void for_each(usize size, T fn)
{
    if constexpr(std::is_void_v<decltype(fn())>)
    {
        for([[maybe_unused]] auto _ : range(size))
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
    for([[maybe_unused]] auto _ : range(size))
    {
        sum += fn();
    }
    return sum / size;
}

using namespace std::chrono;

struct Timer
{
private:
    using f64_ms = duration<f64, std::milli>;

    steady_clock::time_point t0;

public:
    Timer();

    template <typename T>
    static void display(T number, const std::string_view& unit);

    void elapsed() const;

    template <typename T, typename... Args>
    static auto elapsed(T fn, Args&&... args)
    {
        Timer timer;
        if constexpr(std::is_same_v<decltype(fn(std::forward<Args>(args)...)), void>)
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
        if constexpr(std::is_same_v<U, void>)
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
